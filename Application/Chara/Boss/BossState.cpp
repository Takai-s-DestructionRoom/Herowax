#include "BossState.h"
#include "Boss.h"

BossNormal::BossNormal()
{
	priority = 0;
}

void BossNormal::Update(Boss* boss)
{
	boss->SetStateStr("Normal");
}
