/*
* @file SRConstBuffer.h
* @brief SRBufferを使用する定数バッファ
*/

#pragma once

//外部ヘッダ
#pragma warning(push, 0)
#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#include "RDirectX.h"
#include "Util.h"
#include <chrono>
#include <mutex>
#pragma warning(pop)

#include <SRBuffer.h>

template <typename T>
class SRConstBuffer
{
private:
	static std::recursive_mutex sMutex;
	static std::unordered_map<const void*, size_t> sCountMap;

	static void AddCount(const void* p) {
		std::lock_guard<std::recursive_mutex> lock(sMutex);
		sCountMap[p]++;
	}

	static void SubtractCount(const void* p) {
		std::lock_guard<std::recursive_mutex> lock(sMutex);
		sCountMap[p]--;

		if (sCountMap[p] == 0) {
			sCountMap.erase(p);
		}
	}

	static size_t GetCount(const void* p) {
		std::lock_guard<std::recursive_mutex> lock(sMutex);
		auto itr = sCountMap.find(p);
		if (itr == sCountMap.end()) {
			return 0;
		}
		return sCountMap[p];
	}

	void Init() {
		mBuff = SRBufferAllocator::Alloc((sizeof(T) + 0xff) & ~0xff, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		mBuff.SetName("SRConstBuffer");
		*reinterpret_cast<T*>(mBuff.Get()) = T();
	}

public:
	SRBufferPtr mBuff;

	T* Get() {
		return reinterpret_cast<T*>(mBuff.Get());
	}

	T* operator->() {
		return reinterpret_cast<T*>(mBuff.Get());
	}

	SRConstBuffer() {
		Init();
		SRConstBuffer::AddCount(mBuff.GetRegionPtr());
	}
	~SRConstBuffer() {
		if (mBuff != nullptr) {
			SRConstBuffer::SubtractCount(mBuff.GetRegionPtr());
			if (SRConstBuffer::GetCount(mBuff.GetRegionPtr()) == 0) {
				SRBufferAllocator::Free(mBuff);
			}
		}
	}
	
	//コピー
	SRConstBuffer(const SRConstBuffer& o) {
		mBuff = o.mBuff;
		SRConstBuffer::AddCount(mBuff.GetRegionPtr());
	}
	SRConstBuffer& operator=(const SRConstBuffer& o) {
		if (this != &o) {
			if (mBuff != nullptr) {
				SRConstBuffer::SubtractCount(mBuff.GetRegionPtr());
				if (SRConstBuffer::GetCount(mBuff.GetRegionPtr()) == 0) {
					SRBufferAllocator::Free(mBuff);
				}
			}
			mBuff = o.mBuff;
			SRConstBuffer::AddCount(mBuff.GetRegionPtr());
		}
		return *this;
	}
};

template<typename T>
std::unordered_map<const void*, size_t> SRConstBuffer<T>::sCountMap = std::unordered_map<const void*, size_t>();

template<typename T>
std::recursive_mutex SRConstBuffer<T>::sMutex;
