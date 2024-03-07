#pragma once
#include "GameObject.h"
#include "Easing.h"

//蝋
class Wax : public GameObject
{
public:
	//------------ 攻撃関連 ------------//
	uint32_t atkPower;			//攻撃力
	Vector3 atkVec;				//攻撃方向
	Vector3 atkOriginPos;		//攻撃の出現位置
	float atkDist;				//攻撃の射出距離
	Vector2 atkRange{};			//攻撃範囲
	float atkSize;				//攻撃範囲の大きさ
	Easing::EaseTimer atkTimer;	//攻撃時間計測用

public:
	Wax();
	//生成時に必要な情報だけもらって独立
	void Init(
		uint32_t power, Vector3 vec, Vector3 originPos,
		float dist, Vector2 range, float size, float time);
	//継承元との齟齬がないように空のやつを宣言だけしておく
	void Init()override {};
	
	void Update()override;
	void Draw()override;
};