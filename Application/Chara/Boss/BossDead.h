#pragma once
#include "BossState.h"
#include <vector>
#include <array>

class BossDead : public BossState
{
public:
	BossDead();
	void Update(Boss* boss)override;

private:
	bool isStart;

	//Easing::EaseTimer Timer;
	Easing::EaseTimer explosionTimer;	//爆発にかかる時間
};