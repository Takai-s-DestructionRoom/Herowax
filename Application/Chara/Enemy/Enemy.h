#pragma once
#include "GameObject.h"
#include "EnemyState.h"
#include "Easing.h"

class Enemy : public GameObject
{
private:
	//------------ 移動関連 ------------//
	Vector3 moveVec;			//移動ベクトル
	float moveSpeed;			//移動速度
	float slowMag;				//減速率
	float slowCoatingMag;		//蝋かけられたときの減速率
	bool isGraund;				//接地しているかフラグ

	//------------ 攻撃関連 ------------//
	bool isAttack;						//攻撃してるかフラグ
	float atkPower;						//攻撃力
	Easing::EaseTimer atkCoolTimer;		//攻撃するまでのクールタイム

	bool isEscape;						//脱出行動してるかフラグ
	float escapePower;					//蝋から脱出する力
	Easing::EaseTimer escapeCoolTimer;	//脱出行動のクールタイム

	//------------ HP関連 ------------//
	float hp;				//現在のヒットポイント
	float maxHP;			//最大HP

	//------------ その他 ------------//
	//追跡する対象(タワーを入れる)
	ModelObj* target = nullptr;

	EnemyState* state;			//状態管理
	std::string stateStr;		//状態を文字列で保存

public:
	Enemy(ModelObj* target_);
	void Init() override;
	void Update() override;
	void Draw() override;

	void Tracking();

	//状態変更
	void ChangeState(EnemyState* newstate);

	// ゲッター //
	//状態文字情報を取得
	std::string GetState() { return stateStr; }
	//脱出行動フラグ取得
	bool GetIsEscape() { return isEscape; }
	//蝋から脱出する力取得
	float GetEscapePower() { return escapePower; }
	//脱出行動のクールタイム取得
	Easing::EaseTimer* GetEscapeCoolTimer() { return &escapeCoolTimer; }

	// セッター //
	//減速率設定
	void SetSlowMag(float mag) { slowMag = mag; }
	//蝋かけられたときの減速率設定
	void SetSlowCoatingMag(float mag) { slowCoatingMag = mag; }
	//攻撃フラグ設定
	void SetIsAlive(bool flag) { isAlive = flag; }
	//攻撃力設定
	void SetAtkPower(float power) { atkPower = power; }
	//脱出行動フラグ設定
	void SetIsEscape(bool flag) { isEscape = flag; }
	//蝋から脱出する力設定
	void SetEscapePower(float power) { escapePower = power; }
	//状態文字情報を設定
	void SetStateStr(std::string str) { stateStr = str; }
};

