/*
* @file IndexBuffer.h
* @brief 旧インデックスバッファ
* 一つごとにCreateCommittedResourceをする
*/

#pragma once

//外部ヘッダ
#pragma warning(push, 0)
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#pragma warning(pop)

class IndexBuffer
{
public:
	IndexBuffer() {};
	IndexBuffer(uint32_t* list, uint32_t size);
	IndexBuffer(std::vector<uint32_t> list);

	void Init(uint32_t* list, uint32_t size);
	void Init(std::vector<uint32_t> list);

	Microsoft::WRL::ComPtr<ID3D12Resource> mBuff = nullptr;
	D3D12_INDEX_BUFFER_VIEW mView{};
};

