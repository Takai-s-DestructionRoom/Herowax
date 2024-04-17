#pragma once
#include "BossState.h"

class Boss;

class BossCollectStandState :
    public BossState
{
public:
	BossCollectStandState();
	void Update(Boss* boss)override;
};

