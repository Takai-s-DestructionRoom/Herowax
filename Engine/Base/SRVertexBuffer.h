/*
* @file SRVertexBuffer.h
* @brief SRBufferを使用する頂点バッファ
*/

#pragma once
#include "Vertex.h"
#include <SRBuffer.h>

class SRVertexBuffer
{
private:
	struct VertexBufferData {
		size_t count = 0;
		uint32_t dataSize = 0;
		uint32_t strideInBytes = 0;
		SRBufferPtr buff;
	};

	static std::recursive_mutex sMutex;

public:
	SRVertexBuffer() {};

	~SRVertexBuffer() {
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
	SRVertexBuffer(const SRVertexBuffer& o) {
		std::lock_guard<std::recursive_mutex> lock(sMutex);
		if (mData != nullptr) {
			mData->count--;
			if (mData->count == 0) {
				SRBufferAllocator::Free(mData->buff);
			}
		}
		mData = o.mData;
		if(mData != nullptr) mData->count++;
	}

	SRVertexBuffer& operator=(const SRVertexBuffer& o) {
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

	bool IsValid() {
		std::lock_guard<std::recursive_mutex> lock(sMutex);
		return mData != nullptr && mData->buff.GetRegionPtr() != nullptr && mData->buff.GetRegionPtr()->region != nullptr;
	}

	operator bool() {
		return IsValid();
	}

	//Vertex(Pos)の配列とその大きさで頂点バッファを作る
	SRVertexBuffer(VertexP* list, uint32_t size);
	//Vertex(Pos)のvectorで頂点バッファを作る
	SRVertexBuffer(std::vector<VertexP> list);

	//Vertex(PosNormalUv)の配列とその大きさで頂点バッファを作る
	SRVertexBuffer(VertexPNU* list, uint32_t size);
	//Vertex(PosNormalUv)のvectorで頂点バッファを作る
	SRVertexBuffer(std::vector<VertexPNU> list);

	//Vertex(Pos)の配列とその大きさで頂点バッファを作る
	void Init(VertexP* list, uint32_t size);
	//Vertex(Pos)のvectorで頂点バッファを作る
	void Init(std::vector<VertexP> list);

	//Vertex(PosNormalUv)の配列とその大きさで頂点バッファを作る
	void Init(VertexPNU* list, uint32_t size);
	//Vertex(PosNormalUv)のvectorで頂点バッファを作る
	void Init(std::vector<VertexPNU> list);

	//Vertex(PosNormalUv)の配列とその大きさで頂点バッファを更新する
	void Update(VertexPNU* list, uint32_t size);

	//任意の頂点データの配列とその大きさで作る
	template<class T>
	void Init(T* list, uint32_t size) {
		std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->sMutex);
		std::lock_guard<std::recursive_mutex> lock2(sMutex);

		if (mData != nullptr && mData->buff.GetRegionPtr() != nullptr) {
			SRBufferAllocator::Free(mData->buff);
		}
		else {
			mData = std::make_shared<VertexBufferData>();
			mData->count++;
		}

		uint32_t dataSize = static_cast<uint32_t>(sizeof(T) * size);

		mData->buff = SRBufferAllocator::Alloc(dataSize, 1);

		T* vertMap = reinterpret_cast<T*>(mData->buff.Get());
		for (uint32_t i = 0; i < size; i++) {
			vertMap[i] = list[i];
		}

		mData->dataSize = dataSize;
		mData->strideInBytes = sizeof(T);
	}

	//任意の頂点データのvectorで作る
	template<class T>
	void Init(std::vector<T> list) {
		std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->sMutex);
		std::lock_guard<std::recursive_mutex> lock2(sMutex);

		if (mData != nullptr && mData->buff.GetRegionPtr() != nullptr) {
			SRBufferAllocator::Free(mData->buff);
		}
		else {
			mData = std::make_shared<VertexBufferData>();
			mData->count++;
		}

		uint32_t dataSize = static_cast<uint32_t>(sizeof(T) * list.size());

		mData->buff = SRBufferAllocator::Alloc(dataSize, 1);

		T* vertMap = reinterpret_cast<T*>(mData->buff.Get());
		for (uint32_t i = 0; i < list.size(); i++) {
			vertMap[i] = list[i];
		}

		mData->dataSize = dataSize;
		mData->strideInBytes = sizeof(T);
	}

	//任意の頂点データの配列とその大きさで更新
	template<class T>
	void Update(T* list, uint32_t size) {
		std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->sMutex);
		std::lock_guard<std::recursive_mutex> lock2(sMutex);
		if (mData == nullptr || mData->buff.GetRegionPtr() == nullptr) {
			Init(list, size);
			return;
		}

		uint32_t dataSize = static_cast<uint32_t>(sizeof(T) * size);

		if (mData->dataSize != dataSize || mData->strideInBytes != sizeof(T)) {
			Init(list, size);
			return;
		}

		T* vertMap = reinterpret_cast<T*>(mData->buff.Get());
		for (uint32_t i = 0; i < size; i++) {
			vertMap[i] = list[i];
		}

		mData->dataSize = dataSize;
		mData->strideInBytes = sizeof(T);
	}

	//任意の頂点データのvectorで更新
	template<class T>
	void Update(std::vector<T> list) {
		std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::GetInstance()->sMutex);
		std::lock_guard<std::recursive_mutex> lock2(sMutex);
		if (mData == nullptr || mData->buff.GetRegionPtr() == nullptr) {
			Init(list);
			return;
		}

		uint32_t dataSize = static_cast<uint32_t>(sizeof(T) * list.size());

		if (mData->dataSize != dataSize || mData->strideInBytes != sizeof(T)) {
			Init(list);
			return;
		}

		T* vertMap = reinterpret_cast<T*>(mData->buff.Get());
		for (uint32_t i = 0; i < list.size(); i++) {
			vertMap[i] = list[i];
		}

		mData->dataSize = dataSize;
		mData->strideInBytes = sizeof(T);
	}

	D3D12_VERTEX_BUFFER_VIEW GetVertView();

private:
	std::shared_ptr<VertexBufferData> mData = nullptr;
};

