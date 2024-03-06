#pragma once
#include "../../Chara/Charactor/Charactor.h"
#include "Easing.h"

//蝋
class Wax
{
public:
	//------------ 攻撃関連 ------------//
	uint32_t atkPower;			//攻撃力
	Vector2 atkRange{};			//攻撃範囲
	float atkSize;				//攻撃範囲の大きさ

	//------------ その他 ------------//
	ModelObj obj;				//オブジェクト
};