#pragma once
#include "GameObject.h"
#include "SimpleParticle.h"
#include "Easing.h"

class Player : public GameObject
{
public:
	//------------ 移動関連 ------------//
	Vector3 moveVec;			//移動ベクトル
	float moveSpeed;			//移動速度
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
	uint32_t hp;				//現在のヒットポイント
	uint32_t maxHP;				//最大HP

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

	//------------ その他 ------------//
	SimpleParticle moveParticle;

public:
	Player();
	void Init()override;
	void Update()override;
	void Draw()override;

	//移動関数
	void MovePad();
	void MoveKey();

	//攻撃
	void Attack();

	//ﾌｧｲｱ
	void Fire();

	Vector3 GetFrontVec();
};