#include "Texture.h"
#include "RDirectX.h"

#pragma warning(push)
#pragma warning(disable: 26813)
#include <DirectXTex.h>
#pragma warning(pop)

#include "Color.h"
#include "Util.h"
#include "PathUtil.h"

using namespace std;
using namespace DirectX;

void TextureManager::Init()
{
	HRESULT result;

	//デスクリプタヒープ(SRV)
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //シェーダーから見える
	srvHeapDesc.NumDescriptors = NUM_SRV_DESCRIPTORS;

	//生成
	mSrvHeap = nullptr;
	result = RDirectX::GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvHeap));
	assert(SUCCEEDED(result));

	RegisterInternal(GetEmptyTexture(), "PreRegisteredTex_Empty");
	RegisterInternal(GetHogeHogeTexture(), "PreRegisteredTex_HogeHoge");
}

Texture TextureManager::GetEmptyTexture()
{
	HRESULT result;

	Texture texture = Texture(D3D12_RESOURCE_STATE_GENERIC_READ);

	const size_t textureWidth = 100;
	const size_t textureHeight = 100;
	const size_t imageDataCount = textureWidth * textureHeight;
	struct UNORMCOLOR {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};
	vector<UNORMCOLOR> imageData;
	imageData.resize(imageDataCount);

	for (size_t i = 0; i < imageDataCount; i++) {
		imageData[i] = { 255, 255, 255, 255 };
	}

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	textureResourceDesc.Width = textureWidth;
	textureResourceDesc.Height = textureHeight;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;

	//生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.mResource)
	);
	assert(SUCCEEDED(result));

	result = texture.mResource->WriteToSubresource(
		0,
		nullptr,
		&imageData[0],
		sizeof(UNORMCOLOR) * textureWidth,
		sizeof(UNORMCOLOR) * imageDataCount
	);

	return texture;
}

Texture TextureManager::GetHogeHogeTexture()
{
	HRESULT result;

	Texture texture = Texture(D3D12_RESOURCE_STATE_GENERIC_READ);

	const size_t textureWidth = 256;
	const size_t textureHeight = 256;
	const size_t imageDataCount = textureWidth * textureHeight;
	vector<Color> imageData;
	imageData.resize(imageDataCount);

	for (size_t i = 0; i < imageDataCount; i++) {
		size_t x = i % textureWidth;
		size_t y = i / textureWidth;
		
		if ((x < textureWidth / 2 && y < textureHeight / 2)
			|| (x >= textureWidth / 2 && y >= textureHeight / 2)) {
			imageData[i] = Color(1, 0, 1, 1);
		}
		else {
			imageData[i] = Color(0, 0, 0, 1);
		}
	}

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureResourceDesc.Width = textureWidth;
	textureResourceDesc.Height = textureHeight;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;

	//生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.mResource)
	);
	assert(SUCCEEDED(result));

	result = texture.mResource->WriteToSubresource(
		0,
		nullptr,
		&imageData[0],
		sizeof(Color) * textureWidth,
		sizeof(Color) * imageDataCount
	);

	return texture;
}

TextureHandle TextureManager::CreateInternal(const Color color, const size_t width, const uint32_t height, const std::string handle)
{
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	HRESULT result;

	Texture texture = Texture(D3D12_RESOURCE_STATE_GENERIC_READ);

	const size_t imageDataCount = width * height;
	vector<Color> imageData;
	imageData.resize(imageDataCount);

	for (size_t i = 0; i < imageDataCount; i++) {
		imageData[i] = color;
	}

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureResourceDesc.Width = width;
	textureResourceDesc.Height = height;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;

	//生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.mResource)
	);
	assert(SUCCEEDED(result));

	result = texture.mResource->WriteToSubresource(
		0,
		nullptr,
		&imageData[0],
		sizeof(Color) * static_cast<UINT>(width),
		sizeof(Color) * static_cast<UINT>(imageDataCount)
	);

	return RegisterInternal(texture, handle);
}

TextureHandle TextureManager::CreateInternal(const Color* pSource, const size_t width, const uint32_t height, const std::string filepath, const std::string handle)
{
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	HRESULT result;

	if (!filepath.empty()) {
		//一回読み込んだことがあるファイルはそのまま返す
		auto itr = find_if(mTextureMap.begin(), mTextureMap.end(), [&](const std::pair<TextureHandle, Texture>& p) {
			return p.second.mFilePath == filepath;
			});
		if (itr != mTextureMap.end()) {
			return itr->first;
		}
	}

	Texture texture = Texture(D3D12_RESOURCE_STATE_GENERIC_READ);
	texture.mFilePath = filepath;

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
	textureResourceDesc.Width = width;
	textureResourceDesc.Height = height;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.SampleDesc.Count = 1;

	//生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.mResource)
	);
	if (FAILED(result)) {
		return "FailedTextureHandle";
	}

	//てんそー
	result = texture.mResource->WriteToSubresource(
		0,
		nullptr, //全領域へコピー
		pSource, //元データアドレス
		static_cast<UINT>(sizeof(Color) * width), //1ラインサイズ
		static_cast<UINT>(sizeof(Color) * width * static_cast<UINT64>(height)) //1枚サイズ
	);
	if (FAILED(result)) {
		return "FailedTextureHandle";
	}

	return RegisterInternal(texture, handle);
}

TextureHandle TextureManager::LoadInternal(const std::string filepath, const std::string handle)
{
	std::filesystem::path path = PathUtil::ConvertAbsolute(Util::ConvertStringToWString(filepath));

	std::unique_lock<std::recursive_mutex> lock(mMutex);
	HRESULT result;

	//一回読み込んだことがあるファイルはそのまま返す
	auto itr = find_if(mTextureMap.begin(), mTextureMap.end(), [&](const std::pair<TextureHandle, Texture>& p) {
		return p.second.mFilePath == Util::ConvertWStringToString(path.c_str());
		});
	if (itr != mTextureMap.end()) {
		return itr->first;
	}
	lock.unlock();

	Texture texture = Texture(D3D12_RESOURCE_STATE_GENERIC_READ);
	texture.mFilePath = Util::ConvertWStringToString(path.c_str());

	// 画像イメージデータ
	TexMetadata imgMetadata{};
	ScratchImage scratchImg{};
	// WICテクスチャのロード
	result = LoadFromWICFile(
		path.c_str(),
		WIC_FLAGS_FORCE_RGB,
		&imgMetadata, scratchImg
	);
	if (FAILED(result)) {
		return "FailedTextureHandle";
	}

	// ミップマップ生成
	ScratchImage mipChain{};
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain
	);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		imgMetadata = scratchImg.GetMetadata();
	}
	else {
		return "FailedTextureHandle";
	}

	//読み込んだディフューズテクスチャをSRGBとして扱う
	imgMetadata.format = MakeSRGB(imgMetadata.format);

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = imgMetadata.format;
	textureResourceDesc.Width = imgMetadata.width;
	textureResourceDesc.Height = (UINT)imgMetadata.height;
	textureResourceDesc.DepthOrArraySize = (UINT16)imgMetadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)imgMetadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	//生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.mResource)
	);
	if (FAILED(result)) {
		return "FailedTextureHandle";
	}

	//てんそー
	//全ミップマップについて
	for (size_t i = 0; i < imgMetadata.mipLevels; i++) {
		const Image* img = scratchImg.GetImage(i, 0, 0);
		result = texture.mResource->WriteToSubresource(
			(UINT)i,
			nullptr, //全領域へコピー
			img->pixels, //元データアドレス
			(UINT)img->rowPitch, //1ラインサイズ
			(UINT)img->slicePitch //1枚サイズ
		);
		if (FAILED(result)) {
			return "FailedTextureHandle";
		}
	}

	return RegisterInternal(texture, handle);
}

TextureHandle TextureManager::LoadInternal(const void* pSource, const size_t size, const std::string filepath, const std::string handle)
{
	std::unique_lock<std::recursive_mutex> lock(mMutex);
	HRESULT result;

	if (!filepath.empty()) {
		//一回読み込んだことがあるファイルはそのまま返す
		auto itr = find_if(mTextureMap.begin(), mTextureMap.end(), [&](const std::pair<TextureHandle, Texture>& p) {
			return p.second.mFilePath == filepath;
			});
		if (itr != mTextureMap.end()) {
			return itr->first;
		}
	}
	lock.unlock();

	Texture texture = Texture(D3D12_RESOURCE_STATE_GENERIC_READ);
	texture.mFilePath = filepath;

	// 画像イメージデータ
	TexMetadata imgMetadata{};
	ScratchImage scratchImg{};
	// WICテクスチャのロード
	result = LoadFromWICMemory(
		pSource,
		size,
		WIC_FLAGS_NONE,
		&imgMetadata, scratchImg
	);
	if (FAILED(result)) {
		return "FailedTextureHandle";
	}

	// ミップマップ生成
	ScratchImage mipChain{};
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain
	);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		imgMetadata = scratchImg.GetMetadata();
	}
	else {
		return "FailedTextureHandle";
	}

	//読み込んだディフューズテクスチャをSRGBとして扱う
	imgMetadata.format = MakeSRGB(imgMetadata.format);

	// テクスチャバッファ
	// ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = imgMetadata.format;
	textureResourceDesc.Width = imgMetadata.width;
	textureResourceDesc.Height = (UINT)imgMetadata.height;
	textureResourceDesc.DepthOrArraySize = (UINT16)imgMetadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)imgMetadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	//生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.mResource)
	);
	if (FAILED(result)) {
		return "FailedTextureHandle";
	}

	//てんそー
	//全ミップマップについて
	for (size_t i = 0; i < imgMetadata.mipLevels; i++) {
		const Image* img = scratchImg.GetImage(i, 0, 0);
		result = texture.mResource->WriteToSubresource(
			(UINT)i,
			nullptr, //全領域へコピー
			img->pixels, //元データアドレス
			(UINT)img->rowPitch, //1ラインサイズ
			(UINT)img->slicePitch //1枚サイズ
		);
		if (FAILED(result)) {
			return "FailedTextureHandle";
		}
	}

	return RegisterInternal(texture, handle);
}

Texture& TextureManager::GetInternal(const TextureHandle& handle)
{
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	if (handle.empty()) {
		return mTextureMap["PreRegisteredTex_Empty"];
	}

	auto itr = mTextureMap.find(handle);
	if (itr != mTextureMap.end()) {
		return itr->second;
	}
	
	return mTextureMap["PreRegisteredTex_HogeHoge"];
}

TextureHandle TextureManager::RegisterInternal(Texture texture, TextureHandle handle)
{
	std::unique_lock<std::recursive_mutex> lock(mMutex);
	uint32_t useIndex = UINT32_MAX;

	auto itr = mTextureMap.find(handle);
	if (itr != mTextureMap.end()) {
		useIndex = itr->second.mHeapIndex;
	}
	else {
		for (UINT i = 0; i < TextureManager::NUM_SRV_DESCRIPTORS; i++) {
			bool ok = true;
			for (std::pair<const TextureHandle, Texture>& p : mTextureMap) {
				if (p.second.mHeapIndex == i) {
					ok = false;
					break;
				}
			}

			if (ok) {
				useIndex = i;
				break;
			}
		}
	}
	lock.unlock();

	if (useIndex == UINT32_MAX) {
		//over
		return TextureHandle();
	}

	//シェーダーリソースビュー
	D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle = mSrvHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle = mSrvHeap->GetGPUDescriptorHandleForHeapStart();
	size_t incrementSize = RDirectX::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	_cpuHandle.ptr += useIndex * incrementSize;
	_gpuHandle.ptr += useIndex * incrementSize;
	texture.mCpuHandle = _cpuHandle;
	texture.mGpuHandle = _gpuHandle;
	texture.mHeapIndex = useIndex;

	//シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = texture.mResource->GetDesc().Format;
	if (srvDesc.Format == DXGI_FORMAT_R32_TYPELESS) {
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	}
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texture.mResource->GetDesc().MipLevels;

	//生成
	RDirectX::GetDevice()->CreateShaderResourceView(texture.mResource.Get(), &srvDesc, _cpuHandle);

	if (handle.empty()) {
		handle = "NoNameHandle_" + to_string(useIndex);
	}

	texture.mResource->SetName(Util::ConvertStringToWString(handle).c_str());

	lock.lock();
	mTextureMap[handle] = texture;
	return handle;
}

void TextureManager::UnRegisterInternal(const TextureHandle& handle)
{
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	mTextureMap.erase(handle);
}

void TextureManager::UnRegisterAtEndFrameInternal(const TextureHandle& handle)
{
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	mUnregisterScheduledList.push_back(handle);
}

void TextureManager::EndFrameProcessInternal()
{
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	for (TextureHandle& handle : mUnregisterScheduledList) {
		UnRegisterInternal(handle);
	}

	mUnregisterScheduledList.clear();
}

TextureHandle TextureManager::Create(const Color color, const size_t width, const uint32_t height, const std::string handle)
{
	TextureManager* manager = TextureManager::GetInstance();
	return manager->CreateInternal(color, width, height, handle);
}

TextureHandle TextureManager::Create(const Color* pSource, const size_t width, const uint32_t height, const std::string filepath, const std::string handle)
{
	TextureManager* manager = TextureManager::GetInstance();
	return manager->CreateInternal(pSource, width, height, filepath, handle);
}

TextureHandle TextureManager::Load(const string filepath, const std::string handle)
{
	TextureManager* manager = TextureManager::GetInstance();
	return manager->LoadInternal(filepath, handle);
}

TextureHandle TextureManager::Load(const void* pSource, const size_t size, const std::string filepath, const std::string handle)
{
	TextureManager* manager = TextureManager::GetInstance();
	return manager->LoadInternal(pSource, size, filepath, handle);
}

Texture& TextureManager::Get(const TextureHandle& handle)
{
	TextureManager* manager = TextureManager::GetInstance();
	return manager->GetInternal(handle);
}

TextureHandle TextureManager::Register(Texture texture, TextureHandle handle)
{
	TextureManager* manager = TextureManager::GetInstance();
	return manager->RegisterInternal(texture, handle);
}

void TextureManager::UnRegister(const TextureHandle& handle) {
	TextureManager* manager = TextureManager::GetInstance();
	manager->UnRegisterInternal(handle);
}

void TextureManager::UnRegisterAtEndFrame(const TextureHandle& handle)
{
	TextureManager* manager = TextureManager::GetInstance();
	manager->UnRegisterAtEndFrameInternal(handle);
}

void TextureManager::EndFrameProcess()
{
	TextureManager* manager = TextureManager::GetInstance();
	manager->EndFrameProcessInternal();
}

void TextureManager::UnRegisterAll()
{
	TextureManager* manager = TextureManager::GetInstance();
	manager->mTextureMap.clear();
}

void Texture::ChangeResourceState(D3D12_RESOURCE_STATES state)
{
	if (mState == state) return;
	D3D12_RESOURCE_BARRIER barrierDesc{};
	barrierDesc.Transition.pResource = mResource.Get();
	barrierDesc.Transition.StateBefore = mState;
	barrierDesc.Transition.StateAfter = state;

	RDirectX::GetCommandList()->ResourceBarrier(1, &barrierDesc);
	mState = state;
}

void Texture::Copy(Texture* dest, RRect srcRect, Vector2 destPos)
{
	D3D12_TEXTURE_COPY_LOCATION locA{};
	locA.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	locA.pResource = mResource.Get();
	locA.SubresourceIndex = 0;

	D3D12_TEXTURE_COPY_LOCATION locB{};
	locB.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	locB.pResource = dest->mResource.Get();
	locB.SubresourceIndex = 0;

	D3D12_BOX box{};
	box.left = srcRect.left;
	box.right = srcRect.right;
	box.top = srcRect.top;
	box.bottom = srcRect.bottom;
	box.front = 0;
	box.back = 1;

	D3D12_RESOURCE_STATES oldStateA = mState;
	D3D12_RESOURCE_STATES oldStateB = dest->mState;

	ChangeResourceState(D3D12_RESOURCE_STATE_COPY_SOURCE);
	dest->ChangeResourceState(D3D12_RESOURCE_STATE_COPY_DEST);
	
	RDirectX::GetCommandList()->CopyTextureRegion(&locB, 0, 0, 0, &locA, &box);

	ChangeResourceState(oldStateA);
	dest->ChangeResourceState(oldStateB);
}
