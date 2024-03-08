#include "Temperature.h"
#include "Texture.h"
#include "InstantDrawer.h"
#include "RImGui.h"
#include "TimeManager.h"

TemperatureManager* TemperatureManager::GetInstance()
{
	static TemperatureManager instance;
	return &instance;
}

TemperatureManager::TemperatureManager() :
	temperature(0),downSpeed(10)
{

}

void TemperatureManager::Update()
{
	temperature -= downSpeed * TimeManager::deltaTime;
	temperature = Util::Clamp(temperature, MIN_TEMPERATURE, MAX_TEMPERATURE);

	ui.Update();

#pragma region ImGui
	ImGui::SetNextWindowSize({ 350, 210 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("TemperatureUI", NULL, window_flags);

	ImGui::SliderFloat("温度", &temperature, MIN_TEMPERATURE, MAX_TEMPERATURE);
	ImGui::PushItemWidth(120);	//入力の枠小さくするやつ
	ImGui::InputFloat("一定時間(1秒)ごとの低下量", &downSpeed,1.f);
	ImGui::PopItemWidth();		//入力の枠元に戻すやつ
	if (ImGui::TreeNode("温度:詳細設定"))
	{
		ImGui::InputFloat("最低温度", &MIN_TEMPERATURE,1.f);
		ImGui::InputFloat("最大温度", &MAX_TEMPERATURE,1.f);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("UI:詳細設定"))
	{
		ImGui::SliderFloat("X座標", &ui.position.x, 0, Util::WIN_WIDTH);
		ImGui::SliderFloat("Y座標", &ui.position.y, 0, Util::WIN_HEIGHT);
		ImGui::SliderFloat("Xのスケール", &ui.size.x, 0, 10.f);
		ImGui::SliderFloat("Yのスケール", &ui.size.y, 0, 10.f);

		ImGui::TreePop();
	}

	ImGui::End();
#pragma endregion
}

void TemperatureManager::Draw()
{
	ui.Draw();
}

TemperatureUI::TemperatureUI() : 
	position(200,200),size(0.5f,0.5f)
{
}

void TemperatureUI::LoadResource()
{
	TextureManager::Load("./Resources/ondkei.png", "ondkei");
	TextureManager::Load("./Resources/hari.png", "hari");
}

void TemperatureUI::Update()
{
	angle = TemperatureManager::GetInstance()->GetTemperature() - 
		TemperatureManager::GetInstance()->MAX_TEMPERATURE / 2;
}

void TemperatureUI::Draw()
{
	InstantDrawer::DrawGraph(position.x, position.y, size.x, size.y,0, "ondkei");
	InstantDrawer::DrawGraph(position.x, position.y, size.x, size.y,angle, "hari");
}