#pragma once
#include "GameObject.h"
#include "Easing.h"

class Tower : public GameObject
{
public:
	//------------ HP関連 ------------//
	float hp;				//現在のヒットポイント
	float maxHP;			//最大HP

	//------------ その他 ------------//
	Vector3 oriPos;					//揺れる前の座標
	Vector3 shakeVec;				//揺れベクトル
	float shakePower;				//揺れる力
	Easing::EaseTimer shakeTimer;	//揺れる時間

public:
	Tower();
	~Tower();
	void Init()override;
	void Update()override;
	void Draw()override;

	//揺れる
	void Shake();

	//ダメージくらう
	void Damage(float damage, Vector3 vec);
};