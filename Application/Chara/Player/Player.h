#pragma once
#include "GameObject.h"
#include "SimpleParticle.h"
#include "RingParticle.h"
#include "Easing.h"
#include "PlayerState.h"
#include "PlayerUI.h"
 
class Player : public GameObject
{
public:
	//------------ 移動関連 ------------//
	Vector3 moveVec;			//移動ベクトル
	float moveSpeed;			//移動速度
	float moveAccel;			//移動加速度
	float moveAccelAmount;		//移動加速度の1Fあたりの加算量
	bool isGround;				//接地しているかフラグ
	bool isJumping;					//ジャンプ中かフラグ
	Easing::EaseTimer jumpTimer;	//ジャンプ時間計測用
	float jumpHeight;				//ジャンプしてる高さ
	float maxJumpHeight;			//ジャンプの最高到達点
	float jumpPower;				//ジャンプ力(初速)
	float jumpSpeed;				//ジャンプ速度
	float gravity = 0.098f;			//重力
	Vector3 frontVec{ 0,0,1 };		//正面ベクトル

	//------------ HP関連 ------------//
	float hp;				//現在のヒットポイント
	float maxHP;			//最大HP

	//------------ 攻撃関連 ------------//
	bool isAttack;					//攻撃中かフラグ
	Vector3 atkVec;					//攻撃方向
	float atkHeight;				//攻撃する高さ
	float atkSpeed;					//攻撃の射出速度
	Vector2 atkRange{};				//攻撃範囲
	float atkSize;					//攻撃範囲の大きさ
	uint32_t atkPower;				//攻撃力
	Easing::EaseTimer atkCoolTimer;	//攻撃のクールタイム
	Easing::EaseTimer atkTimer;		//攻撃時間計測用
	Easing::EaseTimer solidTimer;	//固まるまでの時間(調整用)

	bool isMugenAttack = false;		//攻撃中でも次の攻撃を出せるフラグ
	
	//----------- 挑発関連 ------------//
	bool isTauntMode = false;		//ロウを直接当てると敵の追いかける対象が自分に変わるモード

	//----------- 新攻撃(パブロ攻撃)関連 -------------//
	float pabloRange;			//最大射程
	float pabloSideRange;		//横の射程
	float pabloSeparator;		//射程の分割数
	float pabloSpeedMag;		//パブロ攻撃時の移動速度
	float shotDeadZone = 1.0f;

	//------------ 炎関係 ------------//
	float fireGauge;			//炎ゲージ
	float maxFireGauge;			//炎ゲージ最大値
	uint32_t fireStock;			//炎ストック
	uint32_t maxFireStock;		//炎ストック最大値

	//------------ その他 ------------//
	std::unique_ptr<PlayerState> attackState;

	PlayerUI ui;

public:
	Player();
	void Init()override;
	void Update()override;
	void Draw()override;

	//移動関数
	void MovePad();
	void MoveKey();

	//攻撃(ステートに移動)
	void Attack();

	//パブロ攻撃処理のまとめ
	void PabloAttack();

	//ﾌｧｲｱ
	void Fire();

	Vector3 GetFrontVec();

	//状態変更
	template <typename ChangePlayerState>
	void ChangeState() {
		state = std::make_unique<ChangePlayerState>();
	};

	//炎ゲージ溜まる
	void FireGaugeCharge(float gauge);

	// ゲッター //
	//お試し実装:殴った相手が自分を追っかけてくるモード
	bool GetTauntMode() { return isTauntMode; };

	// セッター //
};