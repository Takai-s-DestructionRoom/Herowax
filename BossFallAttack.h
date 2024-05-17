#pragma once
#include "BossState.h"
#include <vector>

class BossFallAttack : public BossState
{
public:
	BossFallAttack();
	void Update(Boss* boss)override;

public:
	bool isActive = false;

private:
	bool isStart;	//最初のフレームかフラグ
};

