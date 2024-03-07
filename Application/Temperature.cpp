#include "Temperature.h"
#include "Texture.h"
#include "InstantDrawer.h"
#include "RImGui.h"

TemperatureManager* TemperatureManager::GetInstance()
{
	static TemperatureManager instance;
	return &instance;
}

void TemperatureManager::UpdateUI()
{
	ui.Update();
}

void TemperatureManager::DrawUI()
{
	ui.Draw();
}

TemperatureUI::TemperatureUI() : 
	position(0,0),size(0.5f,0.5f)
{
}

void TemperatureUI::LoadResource()
{
	TextureManager::Load("./Resources/ondkei.png", "ondkei");
	TextureManager::Load("./Resources/hari.png", "hari");
}

void TemperatureUI::Update()
{
	ImGui();
}

void TemperatureUI::Draw()
{
	InstantDrawer::DrawGraph(position.x, position.y, size.x, size.y,0, "ondkei");
	InstantDrawer::DrawGraph(position.x, position.y, size.x, size.y,90, "hari");
}

void TemperatureUI::ImGui()
{
#pragma region ImGui
	ImGui::SetNextWindowSize({ 100, 210 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("TemperatureUI", NULL, window_flags);

	ImGui::InputFloat("PositionX", &position.x);
	ImGui::InputFloat("PositionY", &position.y);
	ImGui::InputFloat("SizeX", &size.y);
	ImGui::InputFloat("SizeY", &size.y);

	ImGui::End();
#pragma endregion
}
