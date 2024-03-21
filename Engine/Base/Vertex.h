/*
* @file Vertex.h
* @brief 頂点を表すクラス
*/

#pragma once

//外部ヘッダ
#pragma warning(push, 0)
#include <d3d12.h>
#pragma warning(pop)

#include "Vector2.h"
#include "Vector3.h"
#include "Color.h"
#include <vector>

struct VertexP
{
	Vector3 pos;

	VertexP(Vector3 pos = { 0, 0, 0 })
		: pos(pos) {}
};

struct VertexPU
{
	Vector3 pos;
	Vector2 uv;

	VertexPU(Vector3 pos = { 0, 0, 0 }, Vector2 uv = {0, 0})
		: pos(pos), uv(uv) {}
};

class VertexPNU
{
public:
	Vector3 pos;
	Vector3 normal;
	Vector2 uv;

	VertexPNU(Vector3 pos = { 0, 0, 0 }, Vector3 normal = { 0, 0, 1 }, Vector2 uv = { 0, 0 })
		: pos(pos), normal(normal), uv(uv) {}

	bool operator==(const VertexPNU& a) const;

	/// <summary>
	/// 法線ベクトルを計算
	/// </summary>
	/// <param name="list">計算する頂点群</param>
	/// <param name="indexlist">インデックスリスト（三角形、全て時計回りであること）</param>
	/// <param name="indexcount">インデックスの数</param>
	static void CalcNormalVec(VertexPNU list[], const uint32_t indexlist[], const uint32_t indexcount);

	static void CalcNormalVec(std::vector<VertexPNU> list, std::vector<uint32_t> indexlist);
};

//position,scale,colorの3要素
struct VertexParticle
{
	Vector3 pos = { 0, 0, 0 };
	Vector3 rot = { 0, 0, 0 };
	Color color = Color::kWhite;
	float scale = 0.0f;
	float timer = 0.0f;

	//コンストラクタ
	VertexParticle(Vector3 pos = { 0, 0, 0 }, Vector3 rot = { 0, 0, 0 },
		Color color = Color::kWhite, float scale = 0.0f)
		: pos(pos), rot(rot), color(color), scale(scale),timer(timer) {}

	bool operator==(const VertexParticle& a) const;
};