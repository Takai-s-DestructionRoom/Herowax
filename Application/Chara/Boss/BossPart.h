#pragma once
#include "GameObject.h"
#include "Easing.h"
#include "BossPartState.h"
#include "ColPrimitive3D.h"
#include "WaxVisual.h"

class Parts : public GameObject
{
public:
	Vector3 oriPos;	//パーツごとの基準座標

	//---------固まり表現関連---------//
	const int32_t requireWaxSolidCount = 10; //完全に固まるまでに必要なロウのかかり回数

	bool isCollected = false;
	Vector3 collectPos = {0,0,0};

	Easing::EaseTimer collectTimer;	//回収されるまでの時間(参照できるように外で管理)
	
	//見た目用のロウ
	std::vector<std::unique_ptr<WaxVisual>> waxVisual;
	
	//見た目用ロウのテストフラグ
	bool isWaxVisualTest = false;
	
	ColPrimitive3D::AABB boxCol;

private:
	//------------ HP関連 ------------//
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

	//ステート管理
	std::unique_ptr<BossPartState> state;
	std::unique_ptr<BossPartState> nextState;
	bool changingState = false;

	const int32_t COLLIDER_NUM = 1;
	std::array<ColPrimitive3D::Sphere, 1> waxVisualColliders;

	Easing::EaseTimer createTimer = 0.25f;
	
	ModelObj boxColObj;

public:
	Parts();
	~Parts();
	void Init();
	void Update();
	void Draw();

	//---ゲッター系---//
	int32_t GetWaxSolidCount() {return waxSolidCount;}
	//固まってるかどうか
	bool GetIsSolid();

	//---セッター系---//
	//無敵時間設定
	void SetMutekiMaxTime(float time) { mutekiTimer.maxTime_ = time; };

	void DealDamage(int32_t damage);

	void CreateWaxVisual();

	//状態変更
	template <typename ChangePartState>
	void ChangeState() {
		nextState = std::make_unique<ChangePartState>();
		changingState = true;
	};
};

