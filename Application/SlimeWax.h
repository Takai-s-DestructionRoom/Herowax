/*
* @file SlimeWax.h
* @brief レイマーチングを使用した球描画でくっつくロウを表現するクラス
*/

#pragma once
#include "GameObject.h"
#include "BillboardImage.h"
#include "ColPrimitive3D.h"
#include "Float4.h"
#include "GraphicsPipeline.h"
#include "Easing.h"

static const int MAX_SPHERE_COUNT = 256;

//テスト用にランダムに動く球
class RandomSphere
{
public:
	ColPrimitive3D::Sphere collider;
	Easing::EaseTimer timer = 3.0f;
	Vector3 moveVec = { 0,0,0 };
	float moveSpeed = 0.01f;

public:
	void Init();
	void Update();
};

struct SlimeBuffer
{
	float slimeValue;
	int32_t sphereNum;
	int32_t rayMatchNum;
	float clipValue;

	Float4 spheres[MAX_SPHERE_COUNT];
};

class SlimeWax : public GameObject
{
public:
	void Init()override;
	void Update()override;
	void Draw()override;

private:
	GraphicsPipeline SlimeShaderPipeLine();

public:

private:
	BillboardImage screen; //球を移すスクリーンビルボード
	std::vector<RandomSphere> spheres;	//シェーダーに送る球たち
	bool isPlaneDraw = false;	//テスト用 ビルボードを描画するか

	SRConstBuffer<SlimeBuffer> slimeBuff;	//シェーダーで使う情報
	int32_t sphereNum = 32;	//初期生成する球の数

	float slimeValue = 1.0f;	//どのくらい近づいたら球同士がくっつくか
	int32_t rayMatchNum = 16;	//1ピクセル当たりどのくらいレイを飛ばすか
	float clipValue = 1.0f;		//計算結果がどのくらい近かったら描画するか
								//(だいたいリムライトの強さと思ってもらって問題ない)

	//レイマーチの周回数と球の合計数を掛けた値の上限値
	//(シェーダーで1ピクセル当たりにループする回数)
	int32_t rayMarchClampNum = 2048;

	int32_t changeNum = 0;
};

