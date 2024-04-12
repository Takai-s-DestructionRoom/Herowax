#pragma once
#include "GameObject.h"
#include "Easing.h"
#include "Quaternion.h"
#include "Vector2.h"
#include "BossState.h"
#include "BossUI.h"
#include "BossAI.h"
#include "BossPart.h"
#include <array>

enum class PartsNum
{
	RightHand,	//1
	LeftHand,	//0

	Max			//2
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
	Easing::EaseTimer mutekiTimer;			//無敵時間さん

	//------------ その他 ------------//
	std::unique_ptr<BossState> nextState;	//次のステート
	std::string stateStr;					//状態を文字列で保存

	ModelObj* target = nullptr;				//攻撃対象

	BossUI ui;
	BossAI ai;

	bool changingState = false;		//ステート変更中かフラグ
	bool isDrawObj = true;			//オブジェクト描画するかフラグ

	float phaseTimer;	//フェーズ移行に使うタイマー

	//---------死亡表現関連------------//
	Easing::EaseTimer deadTimer;

	//---------被弾時表現関連------------//
	Easing::EaseTimer whiteTimer;	//被弾時に白く光るのを管理する

	//---------固まり表現関連---------//
	int32_t waxSolidCount = 0; //ロウのかかり具合カウント
	int32_t requireWaxSolidCount = 10; //完全に固まるまでに必要なロウのかかり回数
	Easing::EaseTimer waxShakeOffTimer = 5.0f; //ロウを振り払うタイマー

public:
	std::unique_ptr<BossState> state;	//状態管理

	std::array<Parts, 2> parts;			//体のパーツ
	ModelObj targetCircle;				//攻撃位置表示用

	//------------ 行動関連 ------------//
	Easing::EaseTimer standTimer;		//待機時間
	Easing::EaseTimer punchTimer;		//パンチにかかる時間
	Easing::EaseTimer punchStayTimer;	//パンチ終わってからモーション終了までの時間

private:
	//すべてのステートでの共通処理
	void AllStateUpdate();

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
	/// <summary>
	/// 固まっているかどうかを取得
	/// </summary>
	/// <param name="num">どの部位の状態を取得するか maxを指定するとボスの体を取得</param>
	/// <returns>true or false</returns>
	bool GetIsSolid(PartsNum num);

	// セッター //
	//状態文字情報を設定
	void SetStateStr(std::string str) { stateStr = str; }
	//攻撃対象を設定
	void SetTarget(ModelObj* target_) { target = target_; }

	//ダメージを与える
	void DealDamage(int32_t damage);
};