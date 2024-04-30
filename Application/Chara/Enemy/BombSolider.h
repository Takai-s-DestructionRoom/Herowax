#pragma once
#include "Enemy.h"
class BombSolider : public Enemy
{
public:
	BombSolider(ModelObj* target_);

	void Update()override;
};

