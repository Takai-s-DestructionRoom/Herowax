#pragma once
#include "GameObject.h"
#include "Easing.h"
#include "Quaternion.h"
#include "Vector2.h"
#include "BossState.h"
#include "BossUI.h"
#include "BossAI.h"
#include <array>

enum class PartsNum
{
	LeftHand,	//0
	RightHand,	//1

	Max			//2
};

class Parts : public GameObject
{
public:
	Vector3 oriPos;	//パーツごとの基準座標

public:
	void Init();
	void Update();
	void Draw();
};

class Boss : public GameObject
{
private:
	//------------ 移動関連 ------------//
	Vector3 moveVec;			//移動ベクトル
	float moveSpeed;			//移動速度

	//------------ HP関連 ------------//
	float hp;				//現在のヒットポイント
	float maxHP;			//最大HP
	Easing::EaseTimer mutekiTimer;		//無敵時間さん

	//------------ 攻撃関連 ------------//
	Easing::EaseTimer punchTimer;		//パンチにかかる時間
	Easing::EaseTimer stayTimer;		//パンチ終わってからモーション終了までの時間

	//------------ その他 ------------//
	std::unique_ptr<BossState> nextState;	//次のステート
	std::string stateStr;					//状態を文字列で保存

	ModelObj* target = nullptr;				//攻撃対象

	BossUI ui;
	BossAI ai;

	bool changingState = false;		//ステート変更中かフラグ
	bool isDrawObj = true;			//オブジェクト描画するかフラグ

	float phaseTimer;	//フェーズ移行に使うタイマー

public:
	std::unique_ptr<BossState> state;		//状態管理

	std::array<Parts, 2> parts;		//体のパーツ

public:
	Boss();
	virtual ~Boss();
	void Init() override;
	void Update() override;
	void Draw() override;

	// ゲッター //
	//状態文字情報を取得
	std::string GetStateStr() { return stateStr; }
	//攻撃対象を取得
	ModelObj* GetTarget() { return target; }

	// セッター //
	//状態文字情報を設定
	void SetStateStr(std::string str) { stateStr = str; }
	//攻撃対象を設定
	void SetTarget(ModelObj* target_) { target = target_; }
};