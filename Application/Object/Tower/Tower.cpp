#include "Tower.h"
#include "Camera.h"
#include "ImGui.h"

Tower::Tower():
	hp(0), maxHP(5), isAlive(true)
{
	obj = ModelObj(Model::Load("./Resources/Model/stick.obj", "Stick", true));
}

void Tower::Init()
{
	hp = maxHP;
	obj.mTransform.position = { 0.f,0.f,2.f };
	obj.mTransform.rotation.z = Util::AngleToRadian(90.f);
}

void Tower::Update()
{
	UpdateCollider();

	//HP0になったら死ぬ
	if (hp <= 0)
	{
		isAlive = false;
	}

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);

#pragma region ImGui
	ImGui::SetNextWindowSize({ 300, 100 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Tower", NULL, window_flags);

	ImGui::Text("RキーかRボタンでHP減らせます");
	ImGui::Text("HP:%d", hp);

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

void Tower::UpdateCollider()
{
	collider.pos = GetPos();
	//足元に判定をずらす
	collider.pos.y = 0;
	collider.r = 1;
}