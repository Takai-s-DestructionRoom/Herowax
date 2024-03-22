#pragma once
#include "GameObject.h"
#include "SimpleParticle.h"
#include "Easing.h"

//炎出すキャラ
class FireUnit : public GameObject
{
public:
	//------------ transform関係 ------------//
	float size;						//大きさの比率
	Vector3 frontVec{ 0,0,1 };		//正面ベクトル
	Transform target;				//ターゲット
	Vector3 offset;					//プレイヤー座標とのずれ
	Easing::EaseTimer floatingTimer;//ふよふよさせるタイマー

	//------------ 炎関係 ------------//
	float fireGauge;			//炎ゲージ
	float maxFireGauge;			//炎ゲージ最大値
	uint32_t fireStock;			//炎ストック
	uint32_t maxFireStock;		//炎ストック最大値
	bool isFireStock;

public:
	FireUnit();

	void Init();
	void Update();
	void Draw();

	//炎発射
	void FireShot();
	//炎ゲージ溜まる
	void FireGaugeCharge(float gauge);

	// ゲッター //

	// セッター //
	//トランスフォーム設定
	void SetTransform(Transform transform);
	//ストック性にするかフラグ設定
	void SetIsFireStock(bool frag) { isFireStock = frag; }
};