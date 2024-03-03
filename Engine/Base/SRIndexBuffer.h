/*
* @file SRIndexBuffer.h
* @brief SRBufferを使用するインデックスバッファ
*/

#pragma once
#include <vector>
#include <SRBuffer.h>

class SRIndexBuffer {
private:
	struct IndexBufferData {
		size_t count = 0;
		uint32_t dataSize = 0;
		uint32_t indexCount = 0;
		SRBufferPtr buff;
	};

	static std::recursive_mutex sMutex;

public:
	SRIndexBuffer() {};

	~SRIndexBuffer() {
		std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->sMutex);
		std::lock_guard<std::recursive_mutex> lock2(sMutex);
		if (mData != nullptr) {
			mData->count--;
			if (mData->count == 0) {
				SRBufferAllocator::Free(mData->buff);
			}
		}
	}

	//コピー対策
	SRIndexBuffer(const SRIndexBuffer& o) {
		std::lock_guard<std::recursive_mutex> lock(sMutex);
		if (mData != nullptr) {
			mData->count--;
			if (mData->count == 0) {
				SRBufferAllocator::Free(mData->buff);
			}
		}
		mData = o.mData;
		if (mData != nullptr) mData->count++;
	}

	SRIndexBuffer& operator=(const SRIndexBuffer& o) {
		std::lock_guard<std::recursive_mutex> lock(sMutex);
		if (this != &o) {
			if (mData != nullptr) {
				mData->count--;
				if (mData->count == 0) {
					SRBufferAllocator::Free(mData->buff);
				}
			}
			mData = o.mData;
			if (mData != nullptr) mData->count++;
		}
		return *this;
	}

	SRIndexBuffer(uint32_t* list, uint32_t size);
	SRIndexBuffer(std::vector<uint32_t> list);

	void Init(uint32_t* list, uint32_t size);
	void Init(std::vector<uint32_t> list);

	bool IsValid() {
		std::lock_guard<std::recursive_mutex> lock(sMutex);
		return mData != nullptr && mData->buff.GetRegionPtr() != nullptr && mData->buff.GetRegionPtr()->region != nullptr;
	}

	operator bool() {
		return IsValid();
	}

	D3D12_INDEX_BUFFER_VIEW GetIndexView();
	uint32_t GetIndexCount();

private:
	std::shared_ptr<IndexBufferData> mData = nullptr;
};
