#pragma once
#include "GameObject.h"
#include "Easing.h"
#include "WaxState.h"

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

	//------------ 燃焼関連 ------------//
	Color waxOriginColor;		//蝋の元の色
	Color waxEndColor;			//蝋の燃えた後の色
	
	Easing::EaseTimer igniteTimer;	//燃え始めて色が変わるの時間
	Easing::EaseTimer burningTimer;	//蝋が燃え尽きるまでの時間
	Easing::EaseTimer extinguishTimer;	//蝋が燃え尽きて消えるまでの時間
	
	WaxState* state;			//燃焼の状態管理

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

	bool IsBurning();
	bool IsNormal();

	void ChangeState(WaxState* newstate);
};