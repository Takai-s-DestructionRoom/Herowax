#pragma once
#include "Enemy.h"

class Tank : public Enemy
{
public:
	Tank(ModelObj* target_);

	void Update()override;
};

