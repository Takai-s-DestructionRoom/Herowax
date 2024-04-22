#include "EnemyState.h"
#include "Enemy.h"
#include "EnemyManager.h"
#include "Temperature.h"
#include "WaxManager.h"
#include "ParticleManager.h"
#include "Camera.h"
#include "Float4.h"
#include "ColPrimitive3D.h"

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
	priority = 1;
}

void EnemyAllStop::Update(Enemy* enemy)
{
	//タイマー開始
	if (!enemy->solidTimer.GetStarted() && enemy->GetHP() > 0) {
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
	/*if (enemy->solidTimer.GetEnd()) {
		enemy->solidTimer.Reset();
		enemy->ChangeState<EnemyNormal>();
	}*/

	if (enemy->solidTimer.GetTimeRate() > 0.5f &&
		enemy->solidTimer.GetRun()) {

		//シェイクを算出
		enemy->shack = Util::GetRandVector3(enemy->obj.mTransform.position, -1.f, 1.f, { 1,0,1 });

		//この値を移動値に足したいので、移動した分の値だけを算出
		enemy->shack -= enemy->obj.mTransform.position;
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
			enemy->GetCenterPos(), enemy->obj.mTransform.scale * 0.5f, 2, 0.3f,
			Color::kFireOutside, TextureManager::Load("./Resources/fireEffect.png"),
			1.5f, 3.f, { -0.1f,0.1f,-0.1f }, { 0.1f,0.5f,0.1f },
			0.05f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f, 0.f, false, true);
		//中心の炎
		ParticleManager::GetInstance()->AddSimple(
			enemy->GetCenterPos(), enemy->obj.mTransform.scale * 0.3f, 2, 0.2f,
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

EnemyCollect::EnemyCollect()
{
	priority = 2;
}

void EnemyCollect::Update(Enemy* enemy)
{
	enemy->isCollect = true;

	oldTimeRate = enemy->collectTimer.GetTimeRate();
	enemy->collectTimer.Update();
	if (isStart) {
		enemy->collectTimer.Start();
		isStart = false;
		startPos = enemy->obj.mTransform.position;
	}
	enemy->SetStateStr("EnemyCollect");

	Vector3 now = OutQuadVec3(startPos, enemy->collectPos, enemy->collectTimer.GetTimeRate());
	Vector3 old = OutQuadVec3(startPos, enemy->collectPos, oldTimeRate);
	now.y = 0;
	old.y = 0;
	//現在フレームの移動量の分だけをmoveVecに足す
	enemy->MoveVecPlus(now - old);

	//徐々にちっちゃくなる(ロウとまぎれてよくわかんなくなるから消してる)
	//enemy->obj.mTransform.scale = OutQuadVec3(enemy->oriScale, Vector3::ZERO, enemy->collectTimer.GetTimeRate());

	//到達したら殺す
	if (enemy->collectTimer.GetEnd())
	{
		enemy->isAlive = false;
		accel = 0.f;

		//enemyManager::GetInstance()->isCollected = true;
	}
}
