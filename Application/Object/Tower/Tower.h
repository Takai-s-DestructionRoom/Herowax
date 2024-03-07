#pragma once
#include "GameObject.h"

class Tower : public GameObject
{
public:
	//------------ HP関連 ------------//
	uint32_t hp;				//現在のヒットポイント
	uint32_t maxHP;				//最大HP

public:
	Tower();
	void Init()override;
	void Update()override;
	void Draw()override;

	//ダメージくらう
	void Damage(uint32_t damage) { hp -= damage; }
};