#pragma once
#include "GameObject.h"
#include "Easing.h"

class Parts : public GameObject
{
public:
	Vector3 oriPos;	//パーツごとの基準座標

private:
	//------------ HP関連 ------------//
	float hp = 0.0f;				//現在のヒットポイント
	float maxHP = 10.0f;			//最大HP
	Easing::EaseTimer mutekiTimer;			//無敵時間さん

	//---------被弾時表現関連------------//
	Easing::EaseTimer whiteTimer;	//被弾時に白く光るのを管理する

	//---------固まり表現関連---------//
	int32_t waxSolidCount = 0; //ロウのかかり具合カウント
	int32_t requireWaxSolidCount = 10; //完全に固まるまでに必要なロウのかかり回数
	Easing::EaseTimer waxShakeOffTimer = 5.0f; //ロウを振り払うタイマー

public:
	Parts();
	void Init();
	void Update();
	void Draw();

	//---セッター系---//
	void DealDamage(int32_t damage);
};

