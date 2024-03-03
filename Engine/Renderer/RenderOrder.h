/*
* @file RenderOrder.h
* @brief 描画一つ一つを表すクラス
*/

#pragma once

//外部ヘッダ
#pragma warning(push, 0)
#include <functional>
#include <d3d12.h>
#pragma warning(pop)

#include "RootSignature.h"
#include "GraphicsPipeline.h"
#include <Viewport.h>
#include <Rect.h>
#include <Vector3.h>
#include <SRBuffer.h>
#include <SRVertexBuffer.h>
#include <SRIndexBuffer.h>

enum class RootDataType {
	UNDEFINED,
	DATA_32BIT_CONSTANTS,
	DESCRIPTOR_TABLE,
	CBV,
	SRV,
	UAV,
	SRBUFFER_CBV,
	CAMERA,
	LIGHT,
};

struct RootData {
	RootDataType type = RootDataType::UNDEFINED;
	union {
		D3D12_GPU_DESCRIPTOR_HANDLE descriptor;
		D3D12_GPU_VIRTUAL_ADDRESS address;
		SRBufferPtr addressSRBuff;
	};

	RootData() 
		: type(RootDataType::UNDEFINED), address(0) {}
	RootData(D3D12_GPU_DESCRIPTOR_HANDLE& descriptor)
		: type(RootDataType::DESCRIPTOR_TABLE), descriptor(descriptor) {}
	RootData(RootDataType type)
		: type(type), address(0) {}
	RootData(RootDataType type, D3D12_GPU_VIRTUAL_ADDRESS address)
		: type(type), address(address) {}
	RootData(RootDataType type, SRBufferPtr& ptr)
		: type(type), addressSRBuff(ptr) {}
};

struct RenderOrder
{
	Vector3 anchorPoint;
	std::vector<std::string> renderTargets;
	std::vector<Viewport> viewports;
	std::vector<RRect> scissorRects;
	D3D_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	ID3D12RootSignature* mRootSignature = nullptr;
	ID3D12PipelineState* pipelineState = nullptr;
	SRVertexBuffer vertBuff;
	SRIndexBuffer indexBuff;
	SRVertexBuffer instanceVertBuff;
	D3D12_VERTEX_BUFFER_VIEW* vertView = nullptr; //SRVertexBufferを使わない場合はこっちにビューを直で
	D3D12_INDEX_BUFFER_VIEW* indexView = nullptr; //SRIndexBufferを使わない場合はこっちにビューを直で
	D3D12_VERTEX_BUFFER_VIEW* instanceVertView = nullptr;
	uint32_t indexCount = 0;
	uint32_t instanceCount = 1;
	std::vector<RootData> rootData;

	/// <summary>
	/// この描画の直前に実行してほしい処理を渡すと呼びます
	/// ラムダ式がおすすめ
	/// </summary>
	std::function<void()> preCommand;

	/// <summary>
	/// この描画の直後に実行してほしい処理を渡すと呼びます
	/// ラムダ式がおすすめ
	/// </summary>
	std::function<void()> postCommand;

	
};

