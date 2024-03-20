#include "Tower.h"
#include "Temperature.h"
#include "Camera.h"
#include "ParticleManager.h"
#include "ImGui.h"

Tower::Tower() : GameObject(), hp(0), maxHP(10.f),shakeTimer(0.3f),shakePower(1.5f)
{
	obj = ModelObj(Model::Load("./Resources/Model/Birdnest/Birdnest.obj", "Birdnest", true));
}

Tower::~Tower()
{
}

void Tower::Init()
{
	hp = maxHP;
}

void Tower::Update()
{
	if (TemperatureManager::GetInstance()->GetTemperature() >= TemperatureManager::GetInstance()->GetHotBorder())
	{
		obj.mTuneMaterial.mColor = Color::kRed;
	}
	else if (TemperatureManager::GetInstance()->GetTemperature() <= TemperatureManager::GetInstance()->GetColdBorder())
	{
		obj.mTuneMaterial.mColor = Color::kBlue;
	}
	else
	{
		obj.mTuneMaterial.mColor = Color::kWhite;
	}

	Shake();

	UpdateCollider();

	//HP0になったら死ぬ
	if (hp <= 0)
	{
		if (isAlive)
		{
			//死んだ瞬間パーティクル出す
			ParticleManager::GetInstance()->AddSimple(
				obj.mTransform.position, obj.mTransform.scale,
				25, 0.5f, obj.mTuneMaterial.mColor, "", 1.5f, 2.5f,
				{ -0.5f,-0.5f,-0.5f }, { 0.5f,0.5f,0.5f },
				0.05f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.1f);
		}
		isAlive = false;
	}
	else
	{
		isAlive = true;
	}

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);

#pragma region ImGui
	ImGui::SetNextWindowSize({ 150, 100 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Tower", NULL, window_flags);
	ImGui::Text("HP:%f", &hp);
	ImGui::SliderFloat("最大HP", &maxHP,0.f,100.f);
	ImGui::SliderFloat("揺れる力", &shakePower,0.f,5.f);
	if (ImGui::Button("HP減少")) {
		Damage(1.f,Vector3::ONE);
	}

	if (ImGui::Button("当たり判定の描画")) {
		isViewCol = !isViewCol;
	}
	if (ImGui::Button("Reset")) {
		hp = maxHP;
		isAlive = true;
	}

	ImGui::End();
#pragma endregion
}

void Tower::Draw()
{
	if (isAlive)
	{
		obj.Draw();
		if (isViewCol) {
			DrawCollider();
		}
	}
}

void Tower::Shake()
{
	shakeTimer.Update();

	if (shakeTimer.GetStarted() == false)
	{
		oriPos = obj.mTransform.position;
	}

	if (shakeTimer.GetStarted())
	{
		//大きさかけて、タイマーが進むごとに揺れ小さくなってく
		shakeVec *= shakePower * (Easing::OutBack(1.f,0.f,shakeTimer.GetTimeRate()));

		obj.mTransform.position = oriPos + shakeVec;
	}

	if (shakeTimer.GetEnd())
	{
		//終わったら揺れる前の位置に戻してあげて、揺れの値も0に
		shakeVec = Vector3::ZERO;
		obj.mTransform.position = oriPos;

		shakeTimer.Reset();
	}
}

void Tower::Damage(float damage, Vector3 vec)
{
	hp -= damage;
	shakeVec = vec.Normalize();
	shakeTimer.Start();

	//ヒットパーティクル出す
	ParticleManager::GetInstance()->AddSimple(
		obj.mTransform.position, obj.mTransform.scale,
		15, 0.3f, obj.mTuneMaterial.mColor, "", 0.5f, 1.5f,
		shakeVec - Vector3::ONE * 0.3f, shakeVec - Vector3::ONE * 0.3f,
		0.03f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.1f);
}
