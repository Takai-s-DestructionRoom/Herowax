#include "TemperatueUI.h"
#include "Texture.h"
#include "Temperature.h"
#include "InstantDrawer.h"
#include "SimpleDrawer.h"
#include "Parameter.h"
#include "RImGui.h"

TemperatureUI::TemperatureUI() :
	position(200, 200), size(0.5f, 0.5f),color(1,1,1,1),
	coldBorder(50),hotBorder(80), burningBorder(100)
{
	std::map<std::string, std::string> extract = Parameter::Extract("TemperatureUI");

	position.x = std::stof(extract["UI_X座標"]);
	position.y = std::stof(extract["UI_Y座標"]);
	size.y = std::stof(extract["UI_Xスケール"]);
	size.y = std::stof(extract["UI_Yスケール"]);
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

	//枠の色が今の色
	//中身の色は線でハッキリ分かれてる
	ImGui::Begin("温度UI");
	ImGui::SliderFloat("X座標", &position.x, 0, Util::WIN_WIDTH);
	ImGui::SliderFloat("Y座標", &position.y, 0, Util::WIN_HEIGHT);
	ImGui::SliderFloat("Xのスケール", &size.x, 0, 1000.f);
	ImGui::SliderFloat("Yのスケール", &size.y, 0, 1000.f);
	float hoge[3] = { color.r, color.g, color.b };
	ImGui::ColorEdit3("Color", hoge);
	color.r = hoge[0];
	color.g = hoge[1];
	color.b = hoge[2];
	if (ImGui::Button("セーブ"))
	{
		Save();
	}
	ImGui::End();
}

void TemperatureUI::Draw()
{
	//InstantDrawer::DrawGraph(position.x, position.y, size.x, size.y, 0, "ondkei");
	//InstantDrawer::DrawGraph(position.x, position.y, size.x, size.y, angle, "hari");
	
	//ゲージ本体
	SimpleDrawer::DrawBox(position.x, position.y - size.y / 2,
		position.x + (burningBorder - TemperatureManager::GetInstance()->MIN_TEMPERATURE) * size.x,
		position.y + size.y / 2, 0, burningColor, true);
	SimpleDrawer::DrawBox(position.x, position.y - size.y / 2,
		position.x + (hotBorder - TemperatureManager::GetInstance()->MIN_TEMPERATURE) * size.x,
		position.y + size.y / 2, 0, hotColor, true);
	SimpleDrawer::DrawBox(position.x, position.y - size.y / 2,
		position.x + (coldBorder - TemperatureManager::GetInstance()->MIN_TEMPERATURE) * size.x,
		position.y + size.y / 2, 0, coldColor, true);

}

void TemperatureUI::Save()
{
	Parameter::Begin("TemperatureUI");
	Parameter::Save("UI_X座標", position.x);
	Parameter::Save("UI_Y座標", position.y);
	Parameter::Save("UI_Xスケール", size.x);
	Parameter::Save("UI_Yスケール", size.y);
	Parameter::End();
}
