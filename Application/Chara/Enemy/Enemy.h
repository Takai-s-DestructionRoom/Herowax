#pragma once
#include "GameObject.h"
#include "EnemyState.h"

class Enemy : public GameObject
{
private:
	//------------ 移動関連 ------------//
	Vector3 moveVec;			//移動ベクトル
	float moveSpeed;			//移動速度
	float slowMag;				//減速率
	bool isGraund;				//接地しているかフラグ

	//------------ HP関連 ------------//
	uint32_t hp;				//現在のヒットポイント
	uint32_t maxHP;				//最大HP

	//------------ その他 ------------//
	//追跡する対象(タワーを入れる)
	ModelObj* target = nullptr;

	EnemyState* state;			//状態管理

public:
	Enemy(ModelObj* target_);
	void Init() override;
	void Update() override;
	void Draw() override;

	void Tracking();

	//状態の
	void ChangeState(EnemyState* newstate);

	// セッター //
	//移動速度設定
	void SetSpeedMag(float mag) { slowMag = mag; }
};

