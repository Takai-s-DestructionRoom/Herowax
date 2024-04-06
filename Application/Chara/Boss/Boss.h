#pragma once
#include "GameObject.h"
#include "Easing.h"
#include "Quaternion.h"
#include "Vector2.h"
#include "BossState.h"

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

	//------------ その他 ------------//
	std::unique_ptr<BossState> state;			//状態管理
	std::unique_ptr<BossState> nextState;		//次のステート
	std::string stateStr;		//状態を文字列で保存

	bool changingState = false;
	bool isDrawCollider = false;

public:
	Boss();
	virtual ~Boss();
	void Init() override;
	void Update() override;
	void Draw() override;

	// ゲッター //
	//状態文字情報を取得
	std::string GetState() { return stateStr; }

	// セッター //
	//状態文字情報を設定
	void SetStateStr(std::string str) { stateStr = str; }
};