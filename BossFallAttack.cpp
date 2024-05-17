#include "BossFallAttack.h"
#include "Boss.h"
#include "ParticleManager.h"
#include <RAudio.h>

BossFallAttack::BossFallAttack()
{
	isFinished = false;
	isStart = true;
}

void BossFallAttack::Update(Boss* boss)
{
	boss->SetStateStr("FallAttack");	//ステートがわかるように設定しとく


}
