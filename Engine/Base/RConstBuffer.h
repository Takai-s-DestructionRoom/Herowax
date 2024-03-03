/*
* @file RConstBuffer.h
* @brief 旧定数バッファクラス
* 一つごとにCreateCommittedResourceをする
*/

#pragma once

//外部ヘッダ
#pragma warning(push, 0)
#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#pragma warning(pop)

#include "RDirectX.h"
#include "Util.h"
#include <chrono>
#include <mutex>

template <typename T>
class RConstBuffer
{
private:
	static std::recursive_mutex sMutex;
	static std::map<void*, size_t> sCountMap;

	static void AddCount(void* p) {
		std::lock_guard<std::recursive_mutex> lock(sMutex);
		sCountMap[p]++;

#ifdef _RCONSTBUFFER_DEBUG_
		OutputDebugString((std::wstring(L"RConstBuffer(") + Util::ConvertStringToWString(Util::StringFormat("%p", p)) + L") Add      : " + std::to_wstring(countMap[p]) + L"\n").c_str());
#endif
	}

	static void SubtractCount(void* p) {
		std::lock_guard<std::recursive_mutex> lock(sMutex);
		sCountMap[p]--;

#ifdef _RCONSTBUFFER_DEBUG_
		OutputDebugString((std::wstring(L"RConstBuffer(") + Util::ConvertStringToWString(Util::StringFormat("%p", p)) + L") Subtract : " + std::to_wstring(countMap[p]) + L"\n").c_str());
#endif

		if (sCountMap[p] == 0) {
			sCountMap.erase(p);
		}
	}

	static size_t GetCount(void* p) {
		std::lock_guard<std::recursive_mutex> lock(sMutex);
		auto itr = sCountMap.find(p);
		if (itr == sCountMap.end()) {
			return 0;
		}
		return sCountMap[p];
	}

	void Init() {
		HRESULT result;

		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		// リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(T) + 0xff) & ~0xff; //256バイトアラインメント
		cbResourceDesc.Height = 1;
		cbResourceDesc.DepthOrArraySize = 1;
		cbResourceDesc.MipLevels = 1;
		cbResourceDesc.SampleDesc.Count = 1;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		
		// 定数バッファの生成
		result = RDirectX::GetDevice()->CreateCommittedResource(
			&cbHeapProp, //ヒープ設定
			D3D12_HEAP_FLAG_NONE,
			&cbResourceDesc, //リソース設定
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mConstBuff)
		);
		assert(SUCCEEDED(result));
	}

public:
	Microsoft::WRL::ComPtr<ID3D12Resource> mConstBuff = nullptr;
	T* mConstMap = nullptr;

	RConstBuffer() {
		Init();
		Map();
		RConstBuffer::AddCount(mConstBuff.Get());
	}
	RConstBuffer(const RConstBuffer& o) {
		mConstBuff = o.mConstBuff;
		mConstMap = o.mConstMap;
		RConstBuffer::AddCount(mConstBuff.Get());
	}
	~RConstBuffer() {
		if (mConstBuff != nullptr) {
			RConstBuffer::SubtractCount(mConstBuff.Get());
			if (RConstBuffer::GetCount(mConstBuff.Get()) == 0) {
				UnMap();
			}
		}
	}
	
	RConstBuffer& operator=(const RConstBuffer& o) {
		if (this != &o) {
			if (mConstBuff != nullptr) {
				RConstBuffer::SubtractCount(mConstBuff.Get());
			}
			mConstBuff = o.mConstBuff;
			mConstMap = o.mConstMap;
			RConstBuffer::AddCount(mConstBuff.Get());
		}
		return *this;
	}

	void Map() {
		HRESULT result;
		result = mConstBuff->Map(0, nullptr, (void**)&mConstMap); //マッピング
		assert(SUCCEEDED(result));
	}

	void UnMap() {
		mConstBuff->Unmap(0, nullptr);
		mConstMap = nullptr;
	}
};

template<typename T>
std::map<void*, size_t> RConstBuffer<T>::sCountMap = std::map<void*, size_t>();

template<typename T>
std::recursive_mutex RConstBuffer<T>::sMutex;
