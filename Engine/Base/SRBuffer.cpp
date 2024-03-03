#include "SRBuffer.h"
#include "RDirectX.h"
#include <Util.h>

std::recursive_mutex SRBufferAllocator::sMutex;
bool SRBufferAllocator::mOptAutoDeflag = false;
bool SRBufferAllocator::mOptAutoReCreateBuffer = true;

SRBufferAllocator* SRBufferAllocator::GetInstance()
{
	static SRBufferAllocator instance;
	return &instance;
}

size_t Align(size_t location, size_t align) {
	if ((align == 0) || (align & (align - 1))) {
		return 0;
	}
	return (location + (align - 1)) & ~(align - 1);
}

SRBufferPtr SRBufferAllocator::Alloc(size_t needSize, uint32_t align)
{
	SRBufferAllocator* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(instance->sMutex);

	MemoryRegion* reg = nullptr;

	reg = _Alloc(needSize, align, mOptAutoDeflag);
	if (reg == nullptr) return SRBufferPtr();

	instance->mRegionPtrs.emplace_back();
	instance->mRegionPtrs.back().region = reg;
	instance->mRegionPtrs.back().memItr = --instance->mRegionPtrs.end();
	return SRBufferPtr(&instance->mRegionPtrs.back());
}

void SRBufferAllocator::Free(SRBufferPtr& ptr)
{
	SRBufferAllocator* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(instance->sMutex);

	if (ptr.mPtr == nullptr || ptr.mPtr->region == nullptr) {
#ifdef _DEBUG
		OutputDebugStringA(Util::StringFormat("RKEngine WARNING: SRBufferAllocator::Free() : Attempted to free an invalid pointer(%p).\n", ptr.mPtr).c_str());
#endif
		return;
	}

	_Free(ptr);
	//もっと安全に消せるように何とかする
	ptr.mPtr->region = nullptr;
	instance->mRegionPtrs.erase(ptr.mPtr->memItr);
	ptr.mPtr = nullptr;
}

MemoryRegion* SRBufferAllocator::_Alloc(size_t needSize, uint32_t align, bool deflag)
{
	SRBufferAllocator* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(instance->sMutex);

	//確保した領域の先頭アドレスを指すポインタ
	byte* newLoc = nullptr;

	//空き領域から欲しいサイズが収まる領域を探す
	for (MemoryRegion& reg : instance->mFreeRegions) {

		//要求境界にアライメントする
		byte* alignedLoc = reinterpret_cast<byte*>(Align(reinterpret_cast<size_t>(reg.pBegin), align));

		//アライメントに失敗するようならnullのまま返す
		if (alignedLoc == nullptr) {
#ifdef _DEBUG
			OutputDebugStringA("RKEngine ERROR: SRBufferAllocator::_Alloc() : Failed Alignment.\n");
#endif
			return nullptr;
		}

		//アライメントされた後のアドレスから要求サイズが確保できるか確認
		size_t remainSize = size_t(reg.pEnd - alignedLoc + 1);
		if (reg.pEnd < alignedLoc || remainSize < needSize) {
			continue; //できないなら別の空き領域でもう一度
		}

		//できるなら採用
		newLoc = alignedLoc;
		break;
	}

	//確保できなかったら
	if (newLoc == nullptr) {
		if (deflag) {
			//デフラグして再確保を試みる
			instance->DeFlag();
			return _Alloc(needSize, align, false);
		}
		if (mOptAutoReCreateBuffer) {
			//より大きいサイズでバッファを作り直して再確保を試みる
			instance->ResizeBuffer();
			return _Alloc(needSize, align, false);
		}
		//可能な限り頑張ったけど無理だったのでごめんねする
#ifdef _DEBUG
		OutputDebugStringA("RKEngine ERROR: SRBufferAllocator::_Alloc() : Failed Alloc. Out of memory.\n");
#endif
		return nullptr;
	}

	//空き領域情報を編集する
	//確保領域の先頭アドレスが含まれている空き領域を探す
	for (auto itr = instance->mFreeRegions.begin(); itr != instance->mFreeRegions.end(); itr++) {
		MemoryRegion& reg = *itr;
		if (newLoc < reg.pBegin || reg.pEnd < newLoc) {
			continue;
		}

		//あれば、空き領域の先頭から確保領域の先頭までを新たな空き領域として追加する
		if (size_t(newLoc - reg.pBegin) != 0) {
			itr = instance->mFreeRegions.emplace(itr, reg.pBegin, newLoc - 1);
			itr++;
		}

		//あれば、確保領域の末尾から空き領域の末尾までを新たな空き領域として追加する
		if (size_t(reg.pEnd - (newLoc + needSize - 1)) != 0) {
			itr = instance->mFreeRegions.emplace(itr++, newLoc + needSize, reg.pEnd);
			itr++;
		}

		//空き領域削除
		instance->mFreeRegions.erase(itr);
		break;
	}

	instance->mUsingRegions.push_back(MemoryRegion(newLoc, newLoc + needSize - 1, align));
	instance->mUsingRegions.back().memItr = --instance->mUsingRegions.end();
	instance->mUsingBufferSizeCounter += needSize;
	return &instance->mUsingRegions.back();
}

void SRBufferAllocator::_Free(SRBufferPtr& ptr)
{
	SRBufferAllocator* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(instance->sMutex);

	//使用中領域から指定アドレスを先頭とする領域を探す
	MemoryRegion usingRegion = *ptr.mPtr->region->memItr;
	instance->mUsingRegions.erase(ptr.mPtr->region->memItr);
	//消す時に分かりやすく0xddにする
	for (byte* p = usingRegion.pBegin; p <= usingRegion.pEnd; p++) {
		*p = 0xdd;
	}

	//解放する領域を空き領域に追加する
	auto prev = instance->mFreeRegions.end();
	auto next = instance->mFreeRegions.end();
	for (auto itr = instance->mFreeRegions.begin(); itr != instance->mFreeRegions.end(); itr++) {
		//解放する領域の先頭と連続する一番近い既存の空き領域を探す
		if (itr->pEnd <= usingRegion.pBegin) {
			prev = itr;
		}

		//解放する領域の末尾と連続する一番近い既存の空き領域を探す
		if (usingRegion.pEnd <= itr->pBegin) {
			next = itr;
			break;
		}
	}

	//前方領域がある時
	bool prevMerged = false;
	if (prev != instance->mFreeRegions.end()) {
		//完全に連続するなら
		if (prev->pEnd == usingRegion.pBegin - 1) {
			//融合する
			prev->pEnd = usingRegion.pEnd;
			prev->size = prev->pEnd - prev->pBegin + 1;
			prevMerged = true;
		}
	}

	//後方領域がある時
	bool nextMerged = false;
	if (next != instance->mFreeRegions.end()) {
		//完全に連続するなら
		if (next->pBegin == usingRegion.pEnd + 1) {
			//前方領域と融合済みかで分岐して
			if (prevMerged) {
				//融合済みなら更に融合
				prev->pEnd = next->pEnd;
				prev->size = prev->pEnd - prev->pBegin + 1;
				instance->mFreeRegions.erase(next);
				prevMerged = true;
			}
			else {
				//融合してないなら普通に融合
				next->pBegin = usingRegion.pBegin;
				next->size = next->pEnd - next->pBegin + 1;
				prevMerged = true;
			}
		}
	}

	//融合してないなら孤立した領域なので追加する
	if (!prevMerged && !nextMerged) {
		if (next != instance->mFreeRegions.end()) {
			//後ろに領域があるならその前に挿入
			instance->mFreeRegions.insert(next, MemoryRegion(usingRegion.pBegin, usingRegion.pEnd));
		}
		else {
			//ないならプッシュ
			instance->mFreeRegions.emplace_back(usingRegion.pBegin, usingRegion.pEnd);
		}
	}

	instance->mUsingBufferSizeCounter -= usingRegion.size;
}

void SRBufferAllocator::DeFlag()
{
	std::lock_guard<std::recursive_mutex> lock(sMutex);
	mFreeRegions.clear();
	mFreeRegions.emplace_back(mPtrBufferBegin, mPtrBufferEnd);

	std::map<MemoryRegionPtr*, MemoryRegion> map;
	for (MemoryRegionPtr& ptr : mRegionPtrs) {
		map[&ptr] = MemoryRegion(*ptr.region);
	}
	mUsingRegions.clear();
	mUsingBufferSizeCounter = 0;
	for (MemoryRegionPtr& ptr : mRegionPtrs) {
		MemoryRegion& old = map[&ptr];
		auto newReg = _Alloc(old.size, old.align, false);
		if (newReg == nullptr) {
#ifdef _DEBUG
			OutputDebugStringA("RKEngine ERROR: SRBufferAllocator::DeFlag() : Failed Realloc.\n");
#endif
			continue;
		}
		memmove_s(newReg->pBegin, newReg->size, old.pBegin, old.size);
		ptr.region = newReg;
	}

	for (MemoryRegion& reg : mFreeRegions) {
		for (byte* ptr = reg.pBegin; ptr <= reg.pEnd; ptr++) {
			*ptr = 0xdd;
		}
	}
}

void SRBufferAllocator::ResizeBuffer() {
	std::lock_guard<std::recursive_mutex> lock(sMutex);

	// 再度確保
	Microsoft::WRL::ComPtr<ID3D12Resource> newBuff;
	size_t rebufferSize = GetBufferSize() * 2; //とりあえず2倍のサイズに
	HRESULT result;

	// ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = rebufferSize;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&cbHeapProp, //ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&newBuff)
	);
	assert(SUCCEEDED(result));

	result = newBuff->Map(0, nullptr, (void**)&mPtrBufferBegin); //マッピング
	assert(SUCCEEDED(result));

	mPtrBufferEnd = mPtrBufferBegin + rebufferSize - 1;

	mFreeRegions.clear();
	mFreeRegions.emplace_back(mPtrBufferBegin, mPtrBufferEnd);
	for (byte* ptr = mPtrBufferBegin; ptr <= mPtrBufferEnd; ptr++) {
		*ptr = 0xdd;
	}

	// 確保終わり、既存データ移行

	std::map<MemoryRegionPtr*, MemoryRegion> map;
	for (MemoryRegionPtr& ptr : mRegionPtrs) {
		map[&ptr] = MemoryRegion(*ptr.region);
	}
	mUsingRegions.clear();
	mUsingBufferSizeCounter = 0;
	for (MemoryRegionPtr& ptr : mRegionPtrs) {
		MemoryRegion& old = map[&ptr];
		auto newReg = _Alloc(old.size, old.align, false);
		if (newReg == nullptr) {
#ifdef _DEBUG
			OutputDebugStringA("RKEngine ERROR: SRBufferAllocator::ResizeBuffer() : Failed Realloc.\n");
#endif
			continue;
		}
		memmove_s(newReg->pBegin, newReg->size, old.pBegin, old.size);
		ptr.region = newReg;
	}

	for (MemoryRegion& reg : mFreeRegions) {
		for (byte* ptr = reg.pBegin; ptr <= reg.pEnd; ptr++) {
			*ptr = 0xdd;
		}
	}

	mBuffer = newBuff;
}

SRBufferAllocator::SRBufferAllocator() {
	size_t bufferSize = DEFAULT_SIZE;//(defSize + 0xff) & ~0xff; //256バイトアラインメント

	// 確保
	HRESULT result;

	// ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = bufferSize; 
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&cbHeapProp, //ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mBuffer)
	);
	assert(SUCCEEDED(result));

	result = mBuffer->Map(0, nullptr, (void**)&mPtrBufferBegin); //マッピング
	assert(SUCCEEDED(result));

	mPtrBufferEnd = mPtrBufferBegin + bufferSize - 1;

	mFreeRegions.emplace_back(mPtrBufferBegin, mPtrBufferEnd);
	for (byte* ptr = mPtrBufferBegin; ptr <= mPtrBufferEnd; ptr++) {
		*ptr = 0xdd;
	}
}

byte* SRBufferPtr::Get() {
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::sMutex);
	if (mPtr != nullptr && mPtr->region != nullptr) return mPtr->region->pBegin;
	return nullptr;
}

D3D12_GPU_VIRTUAL_ADDRESS SRBufferPtr::GetGPUVirtualAddress() const
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::sMutex);
	D3D12_GPU_VIRTUAL_ADDRESS address = SRBufferAllocator::GetGPUVirtualAddress();
	address += static_cast<uint32_t>(mPtr->region->pBegin - SRBufferAllocator::GetBufferAddress());
	return address;
}

SRBufferPtr::operator bool() const {
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::sMutex);
	return mPtr != nullptr && mPtr->region != nullptr;
}
