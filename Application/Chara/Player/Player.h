#pragma once
#include "../../Chara/Charactor/Charactor.h"
#include "Easing.h"

class Player : public Charactor
{
public:
	//------------ 移動関連 ------------//
	bool isJumping;				//ジャンプ中かフラグ
	Easing::EaseTimer jumpTimer;//ジャンプ時間計測用
	float jumpHeight;			//ジャンプしてる高さ
	float maxJumpHeight;		//ジャンプの最高到達点
	float jumpPower;			//ジャンプ力(初速)
	float jumpSpeed;			//ジャンプ速度
	float gravity = 0.098f;		//重力
public:
	Player();
	void Init()override;
	void Update()override;
	void Draw()override;

	//移動関数
	void MovePad();
	void MoveKey();
};