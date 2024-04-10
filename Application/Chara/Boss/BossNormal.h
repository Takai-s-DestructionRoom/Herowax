#pragma once
#include "BossState.h"

//通常時
class BossNormal : public BossState
{
public:
	BossNormal();
	void Update(Boss* boss)override;

private:
	Easing::EaseTimer floatingTimer;
};