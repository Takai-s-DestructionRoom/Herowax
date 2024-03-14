#pragma once
#include "GameObject.h"
#include "EnemyState.h"
#include "Easing.h"
#include "WaxGroup.h"
#include "EnemyUI.h"

class Enemy : public GameObject
{
private:
	//------------ 移動関連 ------------//
	Vector3 moveVec;			//移動ベクトル
	float moveSpeed;			//移動速度
	float slowMag;				//減速率
	float slowCoatingMag;		//蝋かけられたときの減速率
	bool isGraund;				//接地しているかフラグ
	float gravity;				//重力
	float groundPos;			//地面座標
	Vector3 knockbackVecS;		//ノックバックする方向
	Vector3 knockbackVecE;		//ノックバックする方向
	Easing::EaseTimer knockbackTimer;	//ノックバックする時間

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

 	std::unique_ptr<EnemyState> state;			//状態管理
	std::string stateStr;		//状態を文字列で保存

	EnemyUI ui;

public:
	WaxGroup* trappedWaxGroup;			//足を取られている対象の蝋を保持

public:

	Enemy(ModelObj* target_);
	~Enemy();
	void Init() override;
	void Update() override;
	void Draw() override;

	void Tracking();

	//追いかける対象を変更
	void SetTarget(ModelObj* target_);

	void SetGroundPos(float groundPos_) {groundPos = groundPos_;}

	//状態変更
	template <typename ChangeEnemyState>
	void ChangeState() {
		state = std::make_unique<ChangeEnemyState>();
	};

	// ゲッター //
	//状態文字情報を取得
	std::string GetState() { return stateStr; }
	//脱出行動フラグ取得
	bool GetIsEscape() { return isEscape; }
	//蝋から脱出する力取得
	float GetEscapePower() { return escapePower; }
	//脱出行動のクールタイム取得
	Easing::EaseTimer* GetEscapeCoolTimer() { return &escapeCoolTimer; }
	//HPの取得
	float GetHP() { return hp; };
	float GetMaxHP() { return maxHP; };

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
	void SetIsEscape(bool flag);
	//蝋から脱出する力設定
	void SetEscapePower(float power) { escapePower = power; }
	//状態文字情報を設定
	void SetStateStr(std::string str) { stateStr = str; }
	//ダメージを与える
	void DealDamage(uint32_t damage);
	//引数があればノックバックもする
	void DealDamage(uint32_t damage,const Vector3& dir);
	//強制的に死亡させる
	void SetDeath();
};

