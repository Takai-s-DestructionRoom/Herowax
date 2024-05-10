/*
* @file VertexBuffer.h
* @brief 旧頂点バッファ
* 一つごとにCreateCommittedResourceをする
*/

#pragma once

//外部ヘッダ
#pragma warning(push, 0)
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#pragma warning(pop)

#include "Vertex.h"

class VertexBuffer
{
public:
	VertexBuffer() {};

	//Vertex(Pos)の配列とその大きさで頂点バッファを作る
	VertexBuffer(VertexP* list, uint32_t size);
	//Vertex(Pos)のvectorで頂点バッファを作る
	VertexBuffer(std::vector<VertexP> list);

	//Vertex(PosNormalUv)の配列とその大きさで頂点バッファを作る
	VertexBuffer(VertexPNU* list, uint32_t size);
	//Vertex(PosNormalUv)のvectorで頂点バッファを作る
	VertexBuffer(std::vector<VertexPNU> list);

	//Vertex(Particle)の配列とその大きさで頂点バッファを作る
	VertexBuffer(VertexParticle3D* list, uint32_t size);
	//Vertex(Particle)のvectorで頂点バッファを作る
	VertexBuffer(std::vector<VertexParticle3D> list);

	//Vertex(Pos)の配列とその大きさで頂点バッファを作る
	void Init(VertexP* list, uint32_t size);
	//Vertex(Pos)のvectorで頂点バッファを作る
	void Init(std::vector<VertexP> list);

	//Vertex(PosNormalUv)の配列とその大きさで頂点バッファを作る
	void Init(VertexPNU* list, uint32_t size);
	//Vertex(PosNormalUv)のvectorで頂点バッファを作る
	void Init(std::vector<VertexPNU> list);

	//Vertex(Particle)の配列とその大きさで頂点バッファを作る
	void Init(VertexParticle3D* list, uint32_t size);
	//Vertex(Particle)のvectorで頂点バッファを作る
	void Init(std::vector<VertexParticle3D> list);

	//Vertex(PosNormalUv)の配列とその大きさで頂点バッファを更新する
	void Update(VertexPNU* list, uint32_t size);

	Microsoft::WRL::ComPtr<ID3D12Resource> mBuff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW mView{};
};

