#include "EnemyState.h"
#include "Enemy.h"

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

	if (enemy->trappedWax->isSolid)
	{
		//遷移
		enemy->ChangeState(new EnemyStop());
	}
	//足とられ状態で蝋が固まったら次へ遷移(当たり判定必要)
	//もしくは足がとられている蝋のポインタを持っておいて、固まっているかを調べる
}

void EnemyStop::Update(Enemy* enemy)
{
	enemy->SetStateStr("Stop");
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
		enemy->GetEscapeCoolTimer()->Reset();
	}

	//足とられ固まり状態で蝋のHPが0になったら次へ(当たり判定不要)
}

void EnemyWaxCoating::Update(Enemy* enemy)
{
	enemy->SetStateStr("WaxCoating");

	//蝋まみれの減速率はimguiでいじったものを基準とするのでここではいじらない
	enemy->SetSlowMag(0.f);

	//蝋まみれ状態で時間経過したら蝋固まり状態へ
}

void EnemyWaxStop::Update(Enemy* enemy)
{
	//蝋固まり状態で時間経過したら解除(当たり判定不要)
	enemy;
}


void EnemyBurning::Update(Enemy* enemy)
{
	enemy->SetStateStr("Burning");

	//減速率いじるかわかんないので保留
}