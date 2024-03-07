#include "VertexBuffer.h"
#include "RDirectX.h"

VertexBuffer::VertexBuffer(VertexP* list, uint32_t size)
{
	Init(list, size);
}

VertexBuffer::VertexBuffer(std::vector<VertexP> list)
{
	Init(list);
}

VertexBuffer::VertexBuffer(VertexPNU* list, uint32_t size)
{
	Init(list, size);
}

VertexBuffer::VertexBuffer(std::vector<VertexPNU> list)
{
	Init(list);
}

VertexBuffer::VertexBuffer(VertexParticle* list, uint32_t size)
{
	Init(list, size);
}

VertexBuffer::VertexBuffer(std::vector<VertexParticle> list)
{
	Init(list);
}

void VertexBuffer::Init(VertexP* list, uint32_t size)
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用

	uint32_t dataSize = static_cast<uint32_t>(sizeof(VertexP) * size);

	//頂点バッファリソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//頂点バッファ生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mBuff)
	);
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応した仮想メモリを取得
	//これは頂点バッファのマッピング
	VertexP* vertMap = nullptr;
	result = mBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (uint32_t i = 0; i < size; i++) {
		vertMap[i] = list[i];
	}
	mBuff->Unmap(0, nullptr);

	//頂点バッファビューの作成
	mView.BufferLocation = mBuff->GetGPUVirtualAddress(); //GPU仮想アドレス
	mView.SizeInBytes = dataSize; //頂点バッファのサイズ
	mView.StrideInBytes = sizeof(VertexP); //頂点一個のサイズ
}

void VertexBuffer::Init(std::vector<VertexP> list)
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用

	uint32_t dataSize = static_cast<uint32_t>(sizeof(VertexP) * list.size());

	//頂点バッファリソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//頂点バッファ生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mBuff)
	);
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応した仮想メモリを取得
	//これは頂点バッファのマッピング
	VertexP* vertMap = nullptr;
	result = mBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (uint32_t i = 0; i < list.size(); i++) {
		vertMap[i] = list[i];
	}
	mBuff->Unmap(0, nullptr);

	//頂点バッファビューの作成
	mView.BufferLocation = mBuff->GetGPUVirtualAddress(); //GPU仮想アドレス
	mView.SizeInBytes = dataSize; //頂点バッファのサイズ
	mView.StrideInBytes = sizeof(VertexP); //頂点一個のサイズ
}

void VertexBuffer::Init(VertexPNU* list, uint32_t size)
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用

	uint32_t dataSize = static_cast<uint32_t>(sizeof(VertexPNU) * size);

	//頂点バッファリソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//頂点バッファ生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mBuff)
	);
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応した仮想メモリを取得
	//これは頂点バッファのマッピング
	VertexPNU* vertMap = nullptr;
	result = mBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (uint32_t i = 0; i < size; i++) {
		vertMap[i] = list[i];
	}
	mBuff->Unmap(0, nullptr);

	//頂点バッファビューの作成
	mView.BufferLocation = mBuff->GetGPUVirtualAddress(); //GPU仮想アドレス
	mView.SizeInBytes = dataSize; //頂点バッファのサイズ
	mView.StrideInBytes = sizeof(VertexPNU); //頂点一個のサイズ
}

void VertexBuffer::Init(std::vector<VertexPNU> list)
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用

	uint32_t dataSize = static_cast<uint32_t>(sizeof(VertexPNU) * list.size());

	//頂点バッファリソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//頂点バッファ生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mBuff)
	);
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応した仮想メモリを取得
	//これは頂点バッファのマッピング
	VertexPNU* vertMap = nullptr;
	result = mBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (uint32_t i = 0; i < list.size(); i++) {
		vertMap[i] = list[i];
	}
	mBuff->Unmap(0, nullptr);

	//頂点バッファビューの作成
	mView.BufferLocation = mBuff->GetGPUVirtualAddress(); //GPU仮想アドレス
	mView.SizeInBytes = dataSize; //頂点バッファのサイズ
	mView.StrideInBytes = sizeof(VertexPNU); //頂点一個のサイズ
}

void VertexBuffer::Init(VertexParticle* list, uint32_t size)
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用

	uint32_t dataSize = static_cast<uint32_t>(sizeof(VertexParticle) * size);

	//頂点バッファリソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//頂点バッファ生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mBuff)
	);
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応した仮想メモリを取得
	//これは頂点バッファのマッピング
	VertexParticle* vertMap = nullptr;
	result = mBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (uint32_t i = 0; i < size; i++) {
		vertMap[i] = list[i];
	}
	mBuff->Unmap(0, nullptr);

	//頂点バッファビューの作成
	mView.BufferLocation = mBuff->GetGPUVirtualAddress(); //GPU仮想アドレス
	mView.SizeInBytes = dataSize; //頂点バッファのサイズ
	mView.StrideInBytes = sizeof(VertexParticle); //頂点一個のサイズ
}

void VertexBuffer::Init(std::vector<VertexParticle> list)
{
	HRESULT result;
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用

	uint32_t dataSize = static_cast<uint32_t>(sizeof(VertexParticle) * list.size());

	//頂点バッファリソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = dataSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//頂点バッファ生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mBuff)
	);
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応した仮想メモリを取得
	//これは頂点バッファのマッピング
	VertexParticle* vertMap = nullptr;
	result = mBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (uint32_t i = 0; i < list.size(); i++) {
		vertMap[i] = list[i];
	}
	mBuff->Unmap(0, nullptr);

	//頂点バッファビューの作成
	mView.BufferLocation = mBuff->GetGPUVirtualAddress(); //GPU仮想アドレス
	mView.SizeInBytes = dataSize; //頂点バッファのサイズ
	mView.StrideInBytes = sizeof(VertexParticle); //頂点一個のサイズ
}

void VertexBuffer::Update(VertexPNU* list, uint32_t size)
{
	HRESULT result;

	uint32_t dataSize = static_cast<uint32_t>(sizeof(VertexPNU) * size);

	assert(mBuff->GetDesc().Width >= dataSize);
	if (mBuff->GetDesc().Width < dataSize) {
		return;
	}

	VertexPNU* vertMap = nullptr;
	result = mBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//全頂点に対して
	for (uint32_t i = 0; i < size; i++) {
		vertMap[i] = list[i];
	}
	mBuff->Unmap(0, nullptr);

	mView.SizeInBytes = dataSize;
	mView.StrideInBytes = sizeof(VertexPNU);
}
