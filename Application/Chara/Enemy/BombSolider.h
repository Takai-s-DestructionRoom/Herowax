#pragma once
#include "Enemy.h"
class BombSolider : public Enemy
{
public:
	BombSolider(ModelObj* target_);

	void Update()override;

	static std::string GetEnemyTag();

public:
	//弾を撃つクールタイム
	float shotCoolTime = 1.0f;
	
	//ターゲットを決めてから撃つまでの時間
	float shotBreachTime = 0.5f;

	Vector3 attackEndPos = {};
};

