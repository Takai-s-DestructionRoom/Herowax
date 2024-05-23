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

static const int MAX_SPHERE_COUNT = 1024;

//テスト用にランダムに動く球
class RandomSphere
{
public:
	ColPrimitive3D::Sphere collider;
	bool isAlive = false;			//生きてるか
	
private:
	Vector3 moveVec = { 0,0,0 };	//移動ベクトル
	float moveSpeed = 0.01f;		//移動速度
	float acceralation = 0.01f;		//落下加速度
	float repercussion = 0.3f;		//跳ね返るときの強さ
	int32_t repercussionNum = 0;	//跳ね返った回数

	Vector3 ground = { 0,0,0 };

public:
	void Init();
	void Update();

	void HitCheck(const ColPrimitive3D::Plane& plane);
};

struct SlimeBuffer
{
	float slimeValue;
	int32_t sphereNum;
	int32_t rayMatchNum;
	float clipValue;

	Float4 spheres[MAX_SPHERE_COUNT];

	Float4 waxColor;
	Float4 rimColor;
	float rimPower;
	float pad[3];
};

class SlimeWax : public GameObject
{
public:
	void Init()override;
	void Update()override;
	void Draw()override;

	void Reset();

private:
	GraphicsPipeline SlimeShaderPipeLine();	//パイプライン生成
	Vector3 GetNearSpherePosition();		//一番カメラに近い球の位置を返す
	void ScreenSizeForce();					//サイズ変更
	void TransferBuffer();					//バッファへ転送
	void ImGui();							//ImGui

public:
	std::vector<RandomSphere> spheres;		//シェーダーに送る球たち

	Color waxColor = Color(0.8f, 0.6f, 0.35f, 1.0f);

	Vector3 sphereCenter = {0,0,0};			//球の生成位置
	Vector3 masterMoveVec = { 0,0,0 };		//これが動くとすべてのオブジェクトが動く
private:
	BillboardImage screen;			//球を映すスクリーンビルボード
	//ModelObj cube;			//球を映すスクリーンビルボード
	bool isPlaneDraw = false;	//テスト用 ビルボードを描画するか

	SRConstBuffer<SlimeBuffer> slimeBuff;	//シェーダーで使う情報
	int32_t sphereNum = 32;	//初期生成する球の数

	float slimeValue = 1.0f;	//どのくらい近づいたら球同士がくっつくか
	int32_t rayMatchNum = 16;	//1ピクセル当たりどのくらいレイを飛ばすか
	float clipValue = 1.0f;		//計算結果がどのくらい近かったら描画するか
								//(だいたいリムライトの強さと思ってもらって問題ない)

	Color rimColor = Color(1.5f, 1.5f, 1.5f, 1.0f);
	float rimPower = 2;

	int32_t changeNum = 0;

	std::vector<Vector3> spline;
};

