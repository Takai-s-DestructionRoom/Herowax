#include "EnemyState.h"
#include "Enemy.h"
#include "EnemyManager.h"

void EnemyNormal::Update(Enemy* enemy)
{
	//減速率どちらも0%
	enemy->SetSlowMag(0.f);
	enemy->SetSlowCoatingMag(0.f);
	enemy->SetStateStr("Normal");

	//ここからの遷移は当たり判定に任せる
}

void EnemySlow::Update(Enemy* enemy)
{
	enemy->SetStateStr("Slow");

	//足とられた時の減速率はimguiでいじったものを基準とするのでここではいじらない
	enemy->SetSlowCoatingMag(0.f);

	//足がとられている蝋のポインタを持っておいて、固まっているかを調べる
	if (enemy->trappedWax->isSolid)
	{
		//付与する力が一度に固まる敵の数だけ強まる
		EnemyManager::GetInstance()->IncrementSolidCombo();
		//抜け出す力を付与する
		enemy->SetEscapePower((float)EnemyManager::GetInstance()->GetSolidCombo());

		//遷移
		enemy->ChangeState(new EnemyFootStop());
	}
}

void EnemyFootStop::Update(Enemy* enemy)
{
	enemy->SetStateStr("FootStop");
	//減速率100%
	enemy->SetSlowMag(1.f);

	enemy->SetIsEscape(false);
	//タイマーを回し続ける
	enemy->GetEscapeCoolTimer()->Update();
	if (enemy->GetEscapeCoolTimer()->GetStarted() == false)
	{
		enemy->GetEscapeCoolTimer()->Start();
	}
	else if (enemy->GetEscapeCoolTimer()->GetEnd())
	{
		enemy->SetIsEscape(true);	//脱出行動をする
		//enemy->trappedWax->Damage(enemy->GetEscapePower());

		enemy->GetEscapeCoolTimer()->Reset();
	}

	//抵抗して蝋のHPが0になったら次へ(当たり判定不要)
	if (enemy->trappedWax->isAlive == false)
	{
		//遷移
		enemy->ChangeState(new EnemyNormal());
	}
}

void EnemyWaxCoating::Update(Enemy* enemy)
{
	enemy->SetStateStr("WaxCoating");

	//蝋まみれの減速率はimguiでいじったものを基準とするのでここではいじらない
	enemy->SetSlowMag(0.f);

	//蝋まみれ状態で時間経過したら蝋固まり状態へ
}

void EnemyAllStop::Update(Enemy* enemy)
{
	enemy->SetStateStr("AllStop");

	enemy->SetIsEscape(false);
	//タイマーを回し続ける
	enemy->GetEscapeCoolTimer()->Update();
	if (enemy->GetEscapeCoolTimer()->GetStarted() == false)
	{
		enemy->GetEscapeCoolTimer()->Start();
	}
	else if (enemy->GetEscapeCoolTimer()->GetEnd())
	{
		enemy->SetIsEscape(true);	//脱出行動をする
		//enemy->trappedWax->Damage(enemy->GetEscapePower());

		enemy->GetEscapeCoolTimer()->Reset();
	}

	//抵抗して蝋のHPが0になったら次へ(当たり判定不要)
	if (enemy->trappedWax->isAlive == false)
	{
		//遷移
		enemy->ChangeState(new EnemyNormal());
	}
}


void EnemyBurning::Update(Enemy* enemy)
{
	enemy->SetStateStr("Burning");

	//減速率いじるかわかんないので保留
}