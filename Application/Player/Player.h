#pragma once
#include "ModelObj.h"
#include "Easing.h"

class Player
{
public:
	//------------ 移動関連 ------------//
	Vector3 moveVec;			//移動ベクトル
	float moveSpeed;			//移動速度
	bool isJumping;				//ジャンプ中かフラグ
	Easing::EaseTimer jumpTimer;//ジャンプ時間計測用
	float jumpHeight;			//ジャンプしてる高さ
	float maxJumpHeight;		//ジャンプの最高到達点
	float jumpPower;			//ジャンプ力(初速)
	float jumpSpeed;			//ジャンプ速度
	bool isGraund;				//接地しているかフラグ
	float gravity = 0.098f;		//重力

	//------------ HP関連 ------------//
	bool isAlive;				//生きてるか否か
	int hp;						//現在のヒットポイント
	int maxHP;					//最大HP

	//------------ その他 ------------//
	ModelObj obj;				//オブジェクト

public:
	Player();
	void Init();
	void Update();
	void Draw();

	//移動関数
	void Move();

	// ゲッター //
	//座標取得
	Vector3 GetPos()const { return obj.mTransform.position; }
	//大きさ取得
	Vector3 GetScale()const { return obj.mTransform.scale; }
	//生きてるかフラグ取得
	bool GetIsAlive()const { return isAlive; }

	// セッター //
	//座標設定
	void SetPos(const Vector3& pos) { obj.mTransform.position = pos; }
	//大きさ設定
	void SetScale(const Vector3& scale) { obj.mTransform.scale = scale; }
	//生きてるかフラグ設定
	void SetIsAlive(bool alive) { isAlive = alive; }

private:
	//コピー禁止
	Player& operator=(const Player&) = delete;
};