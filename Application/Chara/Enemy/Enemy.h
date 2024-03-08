#pragma once
#include "GameObject.h"

class Enemy : public GameObject
{
private:
	//------------ 移動関連 ------------//
	Vector3 moveVec;			//移動ベクトル
	float moveSpeed;			//移動速度
	float speedMag;				//移動速度倍率
	bool isGraund;				//接地しているかフラグ

	//------------ HP関連 ------------//
	uint32_t hp;				//現在のヒットポイント
	uint32_t maxHP;				//最大HP

	//追跡する対象(タワーを入れる)
	ModelObj* target = nullptr;

public:
	Enemy(ModelObj* target_);
	void Init() override;
	void Update() override;
	void Draw() override;

	void Tracking();

	// セッター //
	//移動速度設定
	void SetSpeedMag(float mag) { speedMag = mag; }
};

