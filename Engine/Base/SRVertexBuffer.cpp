#include "SRVertexBuffer.h"

std::recursive_mutex SRVertexBuffer::sMutex;

SRVertexBuffer::SRVertexBuffer(VertexP* list, uint32_t size)
{
	Init(list, size);
}

SRVertexBuffer::SRVertexBuffer(std::vector<VertexP> list)
{
	Init(list);
}

SRVertexBuffer::SRVertexBuffer(VertexPNU* list, uint32_t size)
{
	Init(list, size);
}

SRVertexBuffer::SRVertexBuffer(std::vector<VertexPNU> list)
{
	Init(list);
}

void SRVertexBuffer::Init(VertexP* list, uint32_t size)
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->sMutex);
	std::lock_guard<std::recursive_mutex> lock2(sMutex);

	if (mData != nullptr && mData->buff.GetRegionPtr() != nullptr) {
		SRBufferAllocator::Free(mData->buff);
	}
	else {
		mData = std::make_shared<VertexBufferData>();
		mData->count++;
	}

	uint32_t dataSize = static_cast<uint32_t>(sizeof(VertexP) * size);

	mData->buff = SRBufferAllocator::Alloc(dataSize, 1);

	VertexP* vertMap = reinterpret_cast<VertexP*>(mData->buff.Get());
	for (uint32_t i = 0; i < size; i++) {
		vertMap[i] = list[i];
	}

	mData->dataSize = dataSize;
	mData->strideInBytes = sizeof(VertexP);
}

void SRVertexBuffer::Init(std::vector<VertexP> list)
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->sMutex);
	std::lock_guard<std::recursive_mutex> lock2(sMutex);

	if (mData != nullptr && mData->buff.GetRegionPtr() != nullptr) {
		SRBufferAllocator::Free(mData->buff);
	}
	else {
		mData = std::make_shared<VertexBufferData>();
		mData->count++;
	}

	uint32_t dataSize = static_cast<uint32_t>(sizeof(VertexP) * list.size());

	mData->buff = SRBufferAllocator::Alloc(dataSize, 1);

	VertexP* vertMap = reinterpret_cast<VertexP*>(mData->buff.Get());
	for (uint32_t i = 0; i < list.size(); i++) {
		vertMap[i] = list[i];
	}

	mData->dataSize = dataSize;
	mData->strideInBytes = sizeof(VertexP);
}

void SRVertexBuffer::Init(VertexPNU* list, uint32_t size)
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->sMutex);
	std::lock_guard<std::recursive_mutex> lock2(sMutex);

	if (mData != nullptr && mData->buff.GetRegionPtr() != nullptr) {
		SRBufferAllocator::Free(mData->buff);
	}
	else {
		mData = std::make_shared<VertexBufferData>();
		mData->count++;
	}

	uint32_t dataSize = static_cast<uint32_t>(sizeof(VertexPNU) * size);

	mData->buff = SRBufferAllocator::Alloc(dataSize, 1);

	VertexPNU* vertMap = reinterpret_cast<VertexPNU*>(mData->buff.Get());
	for (uint32_t i = 0; i < size; i++) {
		vertMap[i] = list[i];
	}

	mData->dataSize = dataSize;
	mData->strideInBytes = sizeof(VertexPNU);
}

void SRVertexBuffer::Init(std::vector<VertexPNU> list)
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->sMutex);
	std::lock_guard<std::recursive_mutex> lock2(sMutex);

	if (mData != nullptr && mData->buff.GetRegionPtr() != nullptr) {
		SRBufferAllocator::Free(mData->buff);
	}
	else {
		mData = std::make_shared<VertexBufferData>();
		mData->count++;
	}

	uint32_t dataSize = static_cast<uint32_t>(sizeof(VertexPNU) * list.size());

	mData->buff = SRBufferAllocator::Alloc(dataSize, 1);

	VertexPNU* vertMap = reinterpret_cast<VertexPNU*>(mData->buff.Get());
	for (uint32_t i = 0; i < list.size(); i++) {
		vertMap[i] = list[i];
	}

	mData->dataSize = dataSize;
	mData->strideInBytes = sizeof(VertexPNU);
}

void SRVertexBuffer::Update(VertexPNU* list, uint32_t size)
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->sMutex);
	std::lock_guard<std::recursive_mutex> lock2(sMutex);
	if (mData == nullptr || mData->buff.GetRegionPtr() == nullptr) {
		Init(list, size);
		return;
	}

	uint32_t dataSize = static_cast<uint32_t>(sizeof(VertexPNU) * size);

	if (mData->dataSize != dataSize || mData->strideInBytes != sizeof(VertexPNU)) {
		Init(list, size);
		return;
	}

	VertexPNU* vertMap = reinterpret_cast<VertexPNU*>(mData->buff.Get());
	for (uint32_t i = 0; i < size; i++) {
		vertMap[i] = list[i];
	}

	mData->dataSize = dataSize;
	mData->strideInBytes = sizeof(VertexPNU);
}

D3D12_VERTEX_BUFFER_VIEW SRVertexBuffer::GetVertView()
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->sMutex);
	std::lock_guard<std::recursive_mutex> lock2(sMutex);
	if (mData == nullptr || mData->buff.GetRegionPtr() == nullptr) {
		return D3D12_VERTEX_BUFFER_VIEW();
	}
	D3D12_VERTEX_BUFFER_VIEW view{};
	view.BufferLocation = mData->buff.GetGPUVirtualAddress();
	view.SizeInBytes = mData->dataSize; //頂点バッファのサイズ
	view.StrideInBytes = mData->strideInBytes; //頂点一個のサイズ
	return view;
}
