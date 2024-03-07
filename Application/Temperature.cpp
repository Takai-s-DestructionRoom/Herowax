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
	temperature = Util::Clamp(temperature, 0.f, MAX_TEMPERATURE);

	ui.Update();

#pragma region ImGui
	ImGui::SetNextWindowSize({ 200, 210 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("TemperatureUI", NULL, window_flags);

	ImGui::SliderFloat("温度", &temperature,0.f, MAX_TEMPERATURE);
	ImGui::InputFloat("PositionX", &ui.position.x);
	ImGui::InputFloat("PositionY", &ui.position.y);
	ImGui::InputFloat("SizeX", &ui.size.y);
	ImGui::InputFloat("SizeY", &ui.size.y);

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