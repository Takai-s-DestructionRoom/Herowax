#include "BossCollectStandState.h"
#include "Boss.h"

BossCollectStandState::BossCollectStandState()
{
}

void BossCollectStandState::Update(Boss* boss)
{
	boss->SetStateStr("Collected");
}
