#include "Tower.h"
#include "Camera.h"
#include "ImGui.h"

Tower::Tower(): GameObject(),hp(0), maxHP(5)
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
	//無理やり判定を足元にずらす(カスのコード)
	//どっかで当たり判定可視化出来るようにしたいね
	collider.pos.y = 0;

	//HP0になったら死ぬ
	if (hp <= 0)
	{
		isAlive = false;
	}

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);

#pragma region ImGui
	ImGui::SetNextWindowSize({ 150, 100 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Tower", NULL, window_flags);

	
	ImGui::Text("HP:%d", hp);
	if (ImGui::Button("HP減少")) {
		Damage(1);
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
	}
}