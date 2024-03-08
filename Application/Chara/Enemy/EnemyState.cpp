#include "EnemyState.h"
#include "Enemy.h"

void EnemyNormal::Update(Enemy* enemy)
{
	enemy->SetSpeedMag(0.f);
}

void EnemySlow::Update(Enemy* enemy)
{
	enemy;
	//enemy->SetSpeedMag(0.f);
}

void EnemyStop::Update(Enemy* enemy)
{
	enemy->SetSpeedMag(1.f);
}

void EnemyBurning::Update(Enemy* enemy)
{
	enemy;
}
