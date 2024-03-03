/*
* @file SRBuffer.h
* @brief 自作アロケータを含むバッファ管理クラス群
*/

#pragma once

//外部ヘッダ
#pragma warning(push, 0)
#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#include <list>
#include <mutex>
#pragma warning(pop)

struct MemoryRegion {
	byte* pBegin = nullptr;
	byte* pEnd = nullptr;
	size_t size = 0;
	uint32_t align = 0;
	std::list<MemoryRegion>::iterator memItr;

	MemoryRegion() {}
	MemoryRegion(byte* pBegin, byte* pEnd, uint32_t align = 0)
		: pBegin(pBegin), pEnd(pEnd), size(size_t(pEnd - pBegin + 1)), align(align) {}
};

struct MemoryRegionPtr {
	MemoryRegion* region = nullptr;
	std::list<MemoryRegionPtr>::iterator memItr;
};

class SRBufferPtr {
public:
	SRBufferPtr() {}

	byte* Get();

	const MemoryRegionPtr* GetRegionPtr() const {
		return mPtr;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

	bool operator==(const void* ptr) const {
		return mPtr == ptr;
	}
	bool operator!=(const void* ptr) const {
		return mPtr != ptr;
	}
	bool operator==(const SRBufferPtr& ptr) const {
		return mPtr == ptr.mPtr;
	}
	bool operator!=(const SRBufferPtr& ptr) const {
		return !(*this == ptr);
	}

	operator bool() const;

private:
	friend class SRBufferAllocator;
	SRBufferPtr(MemoryRegionPtr* ptr) : mPtr(ptr) {}
	MemoryRegionPtr* mPtr = nullptr;
};

class SRBufferAllocator
{
public:
	static std::recursive_mutex sMutex; //本当は公開したくないんだけど今は妥協
	static bool mOptAutoDeflag;
	static bool mOptAutoReCreateBuffer;

	static SRBufferAllocator* GetInstance();

	static SRBufferPtr Alloc(size_t needSize, uint32_t align);
	static void Free(SRBufferPtr& ptr);
	
	static D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() {
		std::lock_guard<std::recursive_mutex> lock(GetInstance()->sMutex);
		return GetInstance()->mBuffer->GetGPUVirtualAddress();
	}

	static byte* GetBufferAddress() {
		std::lock_guard<std::recursive_mutex> lock(GetInstance()->sMutex);
		return GetInstance()->mPtrBufferBegin;
	}

	static size_t GetUsingBufferSize() {
		return GetInstance()->mUsingBufferSizeCounter;
	}

	static size_t GetStrictUsingBufferSize() {
		std::lock_guard<std::recursive_mutex> lock(GetInstance()->sMutex);
		size_t size = 0;
		for (auto& itr : GetInstance()->mUsingRegions) {
			size += static_cast<size_t>(itr.size);
		}
		return size;
	}

	static size_t GetBufferSize() {
		std::lock_guard<std::recursive_mutex> lock(GetInstance()->sMutex);
		return static_cast<size_t>(GetInstance()->mPtrBufferEnd - GetInstance()->mPtrBufferBegin + 1);
	}

	void DeFlag();
	void ResizeBuffer();

private:
	constexpr static size_t DEFAULT_SIZE = 1024 * 1024 * 64; //64MB

	Microsoft::WRL::ComPtr<ID3D12Resource> mBuffer = nullptr;
	UINT8* mPtrBufferBegin = nullptr;
	UINT8* mPtrBufferEnd = nullptr;
	std::list<MemoryRegion> mFreeRegions;
	std::list<MemoryRegion> mUsingRegions;
	std::list<MemoryRegionPtr> mRegionPtrs;
	size_t mUsingBufferSizeCounter = 0;

	static MemoryRegion* _Alloc(size_t needSize, uint32_t align, bool deflag);
	static void _Free(SRBufferPtr& ptr);

	SRBufferAllocator();
	SRBufferAllocator(const SRBufferAllocator& a) = delete;
	SRBufferAllocator& operator=(const SRBufferAllocator&) = delete;
};

