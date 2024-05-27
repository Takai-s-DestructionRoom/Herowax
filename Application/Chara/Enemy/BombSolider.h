#pragma once
#include "Enemy.h"
class BombSolider : public Enemy
{
public:
	BombSolider(ModelObj* target_);

	void Update()override;

	static std::string GetEnemyTag();

public:
	//出現
	float shotCoolTime = 1.0f;
};

