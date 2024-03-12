#include "Tower.h"
#include "Camera.h"
#include "ImGui.h"

Tower::Tower(): GameObject(),hp(0), maxHP(5.f)
{
	obj = ModelObj(Model::Load("./Resources/Model/Birdnest/Birdnest.obj", "Birdnest", true));
}

void Tower::Init()
{
	hp = maxHP;
}

void Tower::Update()
{
	UpdateCollider();
	
	//HP0になったら死ぬ
	if (hp <= 0)
	{
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
	ImGui::Text("HP:%d", &hp);
	if (ImGui::Button("HP減少")) {
		Damage(1.f);
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