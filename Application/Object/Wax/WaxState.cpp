#include "WaxState.h"
#include "Wax.h"
#include "Easing.h"
#include "Temperature.h"
#include "WaxManager.h"
#include "ParticleManager.h"

void WaxNormal::Update(Wax* wax)
{
	wax->SetStateStr("Normal");
}

WaxIgnite::WaxIgnite()
{
	//燃えている数カウントに+1
	WaxManager::GetInstance()->isBurningNum++;
	//燃えたときに、すでに燃えている蝋の数に応じてボーナス
	float hoge = TemperatureManager::GetInstance()->GetTemperature();
	TemperatureManager::GetInstance()->TemperaturePlus(
		WaxManager::GetInstance()->GetCalcHeatBonus());
	hoge = TemperatureManager::GetInstance()->GetTemperature();
}

void WaxIgnite::Update(Wax* wax)
{
	wax->SetStateStr("Ignite");

	if (!wax->igniteTimer.GetStarted())
	{
		wax->igniteTimer.Start();

		//燃えた瞬間パーティクル出す
		ParticleManager::GetInstance()->AddRing(
			wax->obj.mTransform.position, 
			15, 0.3f, wax->waxEndColor,
			TextureManager::Load("./Resources/particle_simple.png"),
			2.f, 4.f,1.5f,3.0f, 0.1f, 0.5f,
			-Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.1f,0.f, false, true);
	}

	//色が変わる
	wax->obj.mTuneMaterial.mColor.r = Easing::InQuad(wax->waxOriginColor.r, wax->waxEndColor.r, wax->igniteTimer.GetTimeRate());
	wax->obj.mTuneMaterial.mColor.g = Easing::InQuad(wax->waxOriginColor.g, wax->waxEndColor.g, wax->igniteTimer.GetTimeRate());
	wax->obj.mTuneMaterial.mColor.b = Easing::InQuad(wax->waxOriginColor.b, wax->waxEndColor.b, wax->igniteTimer.GetTimeRate());

	wax->igniteTimer.Update();
	//次へ
	if (wax->igniteTimer.GetEnd()) {
		wax->ChangeState<WaxBurning>();
	}
}

void WaxBurning::Update(Wax* wax)
{
	wax->SetStateStr("Burning");

	//色が変わる
	wax->obj.mTuneMaterial.mColor = wax->waxEndColor;

	//この状態になった瞬間、温度を上昇させる
	if (!wax->burningTimer.GetStarted()) {
		float hoge = TemperatureManager::GetInstance()->GetTemperature();
		TemperatureManager::GetInstance()->TemperaturePlus(
			WaxManager::GetInstance()->heatUpTemperature);
		hoge = TemperatureManager::GetInstance()->GetTemperature();
		wax->burningTimer.Start();
	}
	wax->burningTimer.Update();
	//燃える、この状態で当たったら周囲をigniteにする

	frameCount++;
	if (frameCount >= fireAddFrame)
	{
		//燃えてるときパーティクル出す
		ParticleManager::GetInstance()->AddSimple(
			wax->GetCenterPos(), wax->obj.mTransform.scale * 1.f, 3, 0.4f,
			Color::kFireOutside, TextureManager::Load("./Resources/fireEffect.png"),
			2.5f, 5.f, { -0.1f,0.1f,-0.1f }, { 0.1f,0.5f,0.1f },
			0.05f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.1f, 0.f, false, true);
		//中心の炎
		ParticleManager::GetInstance()->AddSimple(
			wax->GetCenterPos(), wax->obj.mTransform.scale * 1.f, 2, 0.2f,
			Color::kFireInside, TextureManager::Load("./Resources/fireEffect.png"),
			2.5f, 5.f, { -0.1f,0.1f,-0.1f }, { 0.1f,0.4f,0.1f },
			0.01f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f, 0.f, false, true);
		frameCount = 0;
	}

	//次へ
	if (wax->burningTimer.GetEnd()) {
		wax->ChangeState<WaxExtinguish>();
	}
}

void WaxExtinguish::Update(Wax* wax)
{
	wax->SetStateStr("Extinguish");

	wax->disolveValue = wax->extinguishTimer.GetTimeRate();

	//色が変わる
	wax->obj.mTuneMaterial.mColor = wax->waxEndColor;

	if (!wax->extinguishTimer.GetStarted()) {
		wax->extinguishTimer.Start();
		saveScale = wax->GetScale();
	}
	wax->extinguishTimer.Update();
	//縮小する
	//wax->obj.mTransform.scale.x = Easing::OutQuad(saveScale.x, 0, wax->extinguishTimer.GetTimeRate());
	//wax->obj.mTransform.scale.y = Easing::OutQuad(saveScale.y, 0, wax->extinguishTimer.GetTimeRate());
	//wax->obj.mTransform.scale.z = Easing::OutQuad(saveScale.z, 0, wax->extinguishTimer.GetTimeRate());

	//終わったら死亡
	if (wax->extinguishTimer.GetEnd()) {
		wax->SetIsAlive(false);
	}
}

void WaxCollect::Update(Wax* wax)
{
	wax->SetStateStr("WaxCollect");

	WaxManager::GetInstance()->isCollected = false;
	accel += accelAmount;

	Vector3 moveVec =
		wax->collectPos - wax->obj.mTransform.position;
	wax->obj.mTransform.position += moveVec.GetNormalize() * accel;

	//色が変わる
	wax->obj.mTuneMaterial.mColor = Color::kGreen;

	//到達したら殺す
	if (moveVec.Length() <= 2.f)
	{
		wax->DeadParticle();
		wax->isAlive = false;
		accel = 0.f;

		WaxManager::GetInstance()->isCollected = true;
	}
}
