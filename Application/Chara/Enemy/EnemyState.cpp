#include "EnemyState.h"
#include "Enemy.h"
#include "EnemyManager.h"
#include "Temperature.h"
#include "WaxManager.h"
#include "ParticleManager.h"

EnemyNormal::EnemyNormal()
{
	priority = 0;
}

void EnemyNormal::Update(Enemy* enemy)
{
	//減速率どちらも0%
	enemy->SetSlowMag(0.f);
	enemy->SetSlowCoatingMag(0.f);
	enemy->SetStateStr("Normal");

	//ここからの遷移は当たり判定に任せる
}

EnemySlow::EnemySlow()
{
	priority = 0;
}

void EnemySlow::Update(Enemy* enemy)
{
	enemy->SetStateStr("Slow");

	//足とられた時の減速率はimguiでいじったものを基準とするのでここではいじらない
	enemy->SetSlowCoatingMag(0.f);
}

EnemyAllStop::EnemyAllStop()
{
	priority = 0;
}

void EnemyAllStop::Update(Enemy* enemy)
{
	//タイマー開始
	if (!enemy->solidTimer.GetStarted()) {
		enemy->solidTimer.Start();
	}

	enemy->solidTimer.Update();

	enemy->SetStateStr("AllStop");

	enemy->SetIsEscape(false);

	//色を保存
	saveColor = enemy->obj.mTuneMaterial.mColor;
	//ロウの色にして固まってるっぽく
	//enemy->obj.mTuneMaterial.mColor = Wax::waxOriginColor;

	//脱出タイマーが終わったら通常の状態へ戻す
	if (enemy->solidTimer.GetEnd()) {
		enemy->solidTimer.Reset();
		enemy->ChangeState<EnemyNormal>();
	}
}

EnemyBurning::EnemyBurning()
{
	priority = 1;

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

	frameCount++;
	if (frameCount >= fireAddFrame)
	{
		//燃えてるときパーティクル出す
		ParticleManager::GetInstance()->AddSimple(
			enemy->GetCenterPos(), enemy->obj.mTransform.scale * 0.5f, 1, 0.3f,
			Color::kFireOutside, TextureManager::Load("./Resources/fireEffect.png"),
			1.5f, 3.f, { -0.1f,0.1f,-0.1f }, { 0.1f,0.5f,0.1f },
			0.05f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f, 0.f, false, true);
		//中心の炎
		ParticleManager::GetInstance()->AddSimple(
			enemy->GetCenterPos(), enemy->obj.mTransform.scale * 0.3f, 1, 0.2f,
			Color::kFireInside, TextureManager::Load("./Resources/fireEffect.png"),
			2.f, 4.f, { -0.1f,0.1f,-0.1f }, { 0.1f,0.4f,0.1f },
			0.01f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f, 0.f, false, true);

		frameCount = 0;
	}

	//減速率いじるかわかんないので保留

	//燃えて死ぬ
	if (timer.GetEnd()) {
		enemy->SetDeath();
	}
}

int32_t EnemyState::GetPriority()
{
	return priority;
}
