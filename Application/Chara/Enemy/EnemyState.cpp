#include "EnemyState.h"
#include "Enemy.h"

void EnemyNormal::Update(Enemy* enemy)
{
	//減速率どちらも0%
	enemy->SetSlowMag(0.f);
	enemy->SetSlowCoatingMag(0.f);
	enemy->SetStateStr("Normal");
}

void EnemySlow::Update(Enemy* enemy)
{
	enemy->SetStateStr("Slow");

	//足とられた時の減速率はimguiでいじったものを基準とするのでここではいじらない
	enemy->SetSlowCoatingMag(0.f);
}

void EnemyStop::Update(Enemy* enemy)
{
	enemy->SetStateStr("Stop");
	//減速率100%
	enemy->SetSlowMag(1.f);
}

void EnemyWaxCoating::Update(Enemy* enemy)
{
	enemy->SetStateStr("WaxCoating");

	//蝋まみれの減速率はimguiでいじったものを基準とするのでここではいじらない
	enemy->SetSlowMag(0.f);
}

void EnemyBurning::Update(Enemy* enemy)
{
	enemy->SetStateStr("Burning");

	//減速率いじるかわかんないので保留
}
