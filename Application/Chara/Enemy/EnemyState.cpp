#include "EnemyState.h"
#include "Enemy.h"

void EnemyNormal::Update(Enemy* enemy)
{
	enemy->SetSpeedMag(0.f);
	enemy->SetStateStr("Normal");
}

void EnemySlow::Update(Enemy* enemy)
{
	enemy->SetStateStr("Slow");
	//enemy->SetSpeedMag(0.f);
}

void EnemyStop::Update(Enemy* enemy)
{
	enemy->SetStateStr("Stop");
	enemy->SetSpeedMag(1.f);
}

void EnemyBurning::Update(Enemy* enemy)
{
	enemy->SetStateStr("Burning");
}

void EnemyWaxCoating::Update(Enemy* enemy)
{
	enemy->SetStateStr("WaxCoating");
}
