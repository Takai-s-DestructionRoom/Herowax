#pragma once
#include "GameObject.h"

class Enemy : public GameObject
{
public:
	//------------ 移動関連 ------------//
	Vector3 moveVec;			//移動ベクトル
	float moveSpeed;			//移動速度
	bool isGraund;				//接地しているかフラグ

	//------------ HP関連 ------------//
	uint32_t hp;				//現在のヒットポイント
	uint32_t maxHP;				//最大HP

	//追跡する対象(プレイヤーを入れる)
	ModelObj* target = nullptr;

public:
	Enemy(ModelObj* target_);
	void Init() override;
	void Update() override;
	void Draw() override;
};

