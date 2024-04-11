#pragma once
#include "GameObject.h"
#include "Easing.h"

class Parts : public GameObject
{
public:
	Vector3 oriPos;	//パーツごとの基準座標

	//---------固まり表現関連---------//
	const int32_t requireWaxSolidCount = 10; //完全に固まるまでに必要なロウのかかり回数

	bool isCollected = false;

private:
	//------------ HP関連 ------------//
	float hp = 0.0f;				//現在のヒットポイント
	float maxHP = 10.0f;			//最大HP
	Easing::EaseTimer mutekiTimer;			//無敵時間さん

	//---------被弾時表現関連------------//
	Easing::EaseTimer whiteTimer;	//被弾時に白く光るのを管理する

	//---------固まり表現関連---------//
	Easing::EaseTimer waxShakeOffTimer = 5.0f; //ロウを振り払うタイマー
	
	Vector3 shake = {};
	Easing::EaseTimer shakeTimer = 0.5f;		//振り払うときにするシェイクを管理するタイマー
	Easing::EaseTimer waxScatterTimer = 0.1f;	//振り払うときにするシェイクを管理するタイマー

	int32_t waxSolidCount = 0; //ロウのかかり具合カウント

	//ロウを出すためのパラメータ(適当でもいい)
	float solidTime= 0;
	int32_t atkPower = 0;
	float atkRange = 0;
	float atkSize = 0;
	float atkSpeed = 0;
	float atkTime = 0;

public:
	Parts();
	~Parts();
	void Init();
	void Update();
	void Draw();

	//---ゲッター系---//
	int32_t GetWaxSolidCount() {return waxSolidCount;}

	//---セッター系---//
	//無敵時間設定
	void SetMutekiMaxTime(float time) { mutekiTimer.maxTime_ = time; };

	void DealDamage(int32_t damage);
};

