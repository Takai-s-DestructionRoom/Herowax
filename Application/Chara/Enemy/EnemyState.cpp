#include "EnemyState.h"
#include "Enemy.h"
#include "EnemyManager.h"
#include "Temperature.h"
#include "WaxManager.h"

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
	if (enemy->trappedWaxGroup->IsSolid())
	{
		//付与する力が一度に固まる敵の数だけ強まる
		EnemyManager::GetInstance()->IncrementSolidCombo();
		//抜け出す力を付与する
		enemy->SetEscapePower((float)EnemyManager::GetInstance()->GetSolidCombo());

		//遷移
		enemy->ChangeState<EnemyFootStop>();
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
		enemy->trappedWaxGroup->Damage(enemy->GetEscapePower());

		enemy->GetEscapeCoolTimer()->Reset();
	}

	//抵抗して蝋のHPが0になったら次へ(当たり判定不要)
	if (enemy->trappedWaxGroup->GetIsAlive() == false)
	{
		//遷移
		enemy->ChangeState<EnemyNormal>();
	}

	std::string state = enemy->trappedWaxGroup->waxs.back()->GetState();
	if (state != "Normal")
	{
		enemy->ChangeState<EnemyBurning>();
	}
}

void EnemyWaxCoating::Update(Enemy* enemy)
{
	enemy->SetStateStr("WaxCoating");

	//蝋まみれの減速率はimguiでいじったものを基準とするのでここではいじらない
	enemy->SetSlowMag(0.f);
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
		enemy->trappedWaxGroup->Damage(enemy->GetEscapePower());

		enemy->GetEscapeCoolTimer()->Reset();
	}

	//抵抗して蝋のHPが0になったら次へ(当たり判定不要)
	if (enemy->trappedWaxGroup->GetIsAlive() == false)
	{
		//遷移
		enemy->ChangeState<EnemyNormal>();
	}
}

EnemyBurning::EnemyBurning()
{
	timer.Start();

	//燃えている数カウントに+1
	EnemyManager::GetInstance()->IncrementBurningCombo();

	float hoge = TemperatureManager::GetInstance()->GetTemperature();
	//燃え始めたときに、すでに燃えている数に応じて追加で温度上昇
	TemperatureManager::GetInstance()->TemperaturePlus(
		EnemyManager::GetInstance()->GetBurningCombo() * 
		EnemyManager::GetInstance()->GetBurningBonus());
	hoge = TemperatureManager::GetInstance()->GetTemperature();  
}

void EnemyBurning::Update(Enemy* enemy)
{
	timer.Update();
	enemy->SetStateStr("Burning");

	enemy->SetSlowMag(1.f);
	enemy->SetSlowCoatingMag(1.f);

	//色を赤に変更
	enemy->obj.mTuneMaterial.mColor = Color::kRed;

	//減速率いじるかわかんないので保留
	
	//燃えて死ぬ
	if (timer.GetEnd()) {
		enemy->isAlive = false;
	}
}