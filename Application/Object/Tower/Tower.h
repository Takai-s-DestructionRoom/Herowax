#pragma once
#include "GameObject.h"

class Tower : public GameObject
{
public:
	//------------ HP関連 ------------//
	float hp;				//現在のヒットポイント
	float maxHP;			//最大HP

	bool isViewCol = false;

public:
	Tower();
	void Init()override;
	void Update()override;
	void Draw()override;

	//ダメージくらう
	void Damage(float damage) { hp -= damage; }
};