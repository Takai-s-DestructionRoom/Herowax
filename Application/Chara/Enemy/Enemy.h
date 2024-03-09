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
	float slowCoatingMag;		//蝋かけられたときの減速率
	bool isGraund;				//接地しているかフラグ

	//------------ HP関連 ------------//
	uint32_t hp;				//現在のヒットポイント
	uint32_t maxHP;				//最大HP

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

	//状態の
	void ChangeState(EnemyState* newstate);

	// ゲッター //
	std::string GetState() { return stateStr; }

	// セッター //
	//減速率設定
	void SetSlowMag(float mag) { slowMag = mag; }
	//蝋かけられたときの減速率設定
	void SetSlowCoatingMag(float mag) { slowCoatingMag = mag; }
	//状態文字情報を設定
	void SetStateStr(std::string str) { stateStr = str; }
};

