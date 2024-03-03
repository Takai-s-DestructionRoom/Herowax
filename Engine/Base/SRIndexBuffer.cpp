#include "SRIndexBuffer.h"

std::recursive_mutex SRIndexBuffer::sMutex;

SRIndexBuffer::SRIndexBuffer(uint32_t* list, uint32_t size)
{
	Init(list, size);
}

SRIndexBuffer::SRIndexBuffer(std::vector<uint32_t> list)
{
	Init(list);
}

void SRIndexBuffer::Init(uint32_t* list, uint32_t size)
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->sMutex);
	std::lock_guard<std::recursive_mutex> lock2(sMutex);

	if (mData != nullptr && mData->buff.GetRegionPtr() != nullptr) {
		SRBufferAllocator::Free(mData->buff);
	}
	else {
		mData = std::make_shared<IndexBufferData>();
		mData->count++;
	}

	uint32_t dataSize = static_cast<uint32_t>(sizeof(uint32_t) * size);

	mData->buff = SRBufferAllocator::Alloc(dataSize, 1);

	uint32_t* vertMap = reinterpret_cast<uint32_t*>(mData->buff.Get());
	for (uint32_t i = 0; i < size; i++) {
		vertMap[i] = list[i];
	}

	mData->dataSize = dataSize;
	mData->indexCount = size;
}

void SRIndexBuffer::Init(std::vector<uint32_t> list)
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->sMutex);
	std::lock_guard<std::recursive_mutex> lock2(sMutex);

	if (mData != nullptr && mData->buff.GetRegionPtr() != nullptr) {
		SRBufferAllocator::Free(mData->buff);
	}
	else {
		mData = std::make_shared<IndexBufferData>();
		mData->count++;
	}

	uint32_t dataSize = static_cast<uint32_t>(sizeof(uint32_t) * list.size());

	mData->buff = SRBufferAllocator::Alloc(dataSize, 1);

	uint32_t* vertMap = reinterpret_cast<uint32_t*>(mData->buff.Get());
	for (uint32_t i = 0; i < list.size(); i++) {
		vertMap[i] = list[i];
	}

	mData->dataSize = dataSize;
	mData->indexCount = static_cast<uint32_t>(list.size());
}

D3D12_INDEX_BUFFER_VIEW SRIndexBuffer::GetIndexView()
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->sMutex);
	std::lock_guard<std::recursive_mutex> lock2(sMutex);
	if (mData == nullptr || mData->buff.GetRegionPtr() == nullptr) {
		return D3D12_INDEX_BUFFER_VIEW();
	}
	D3D12_INDEX_BUFFER_VIEW view{};
	view.BufferLocation = mData->buff.GetGPUVirtualAddress();
	view.SizeInBytes = mData->dataSize; //頂点バッファのサイズ
	view.Format = DXGI_FORMAT_R32_UINT;
	return view;
}

uint32_t SRIndexBuffer::GetIndexCount()
{
	std::lock_guard<std::recursive_mutex> lock2(sMutex);
	if (mData == nullptr) {
		return 0;
	}
	return mData->indexCount;
}
