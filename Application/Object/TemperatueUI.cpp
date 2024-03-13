#include "TemperatueUI.h"
#include "Texture.h"
#include "Temperature.h"
#include "InstantDrawer.h"
#include "SimpleDrawer.h"
#include "Parameter.h"
#include "RImGui.h"

TemperatureUI::TemperatureUI() :
	position(200, 200), size(0.5f, 0.5f),
	frameSize(1.f,1.f)
{
	std::map<std::string, std::string> extract = Parameter::Extract("TemperatureUI");

	position.x = std::stof(extract["UI_X座標"]);
	position.y = std::stof(extract["UI_Y座標"]);
	size.x = std::stof(extract["UI_Xスケール"]);
	size.y = std::stof(extract["UI_Yスケール"]);
	frameSize.x = std::stof(extract["フレームのサイズX"]);
	frameSize.y = std::stof(extract["フレームのサイズY"]);
	frameColor.r = std::stof(extract["背景色_R"]);
	frameColor.g = std::stof(extract["背景色_G"]);
	frameColor.b = std::stof(extract["背景色_B"]);
	frameColor.a = std::stof(extract["背景色_A"]);
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
	ImGui::SliderFloat("Xのスケール", &size.x, 0, 10);
	ImGui::SliderFloat("Yのスケール", &size.y, 0, 100.f);
	ImGui::SliderFloat("フレームのサイズX", &frameSize.x, 0,100);
	ImGui::SliderFloat("フレームのサイズY", &frameSize.y, 0,100);
	float hoge[4] = { frameColor.r,frameColor.g ,frameColor.b,frameColor.a };
	ImGui::ColorEdit4("背景色変更", hoge);
	frameColor.r = hoge[0];
	frameColor.g = hoge[1];
	frameColor.b = hoge[2];
	frameColor.a = hoge[3];
	if (ImGui::Button("セーブ"))
	{
		Save();
	}
	ImGui::End();

	//minからcoldborderまでがこの色になってほしい
	//結局同じスピードで3つ重ねて動かすのが一番早いんじゃないか？
	//ボーダーでクランプする感じで
	coldPos.x = TemperatureManager::GetInstance()->GetTemperature();
	coldPos.x = Util::Clamp(coldPos.x,0.f, TemperatureManager::GetInstance()->GetColdBorder());

	hotPos.x = TemperatureManager::GetInstance()->GetTemperature();
	hotPos.x = Util::Clamp(hotPos.x, 0.f, TemperatureManager::GetInstance()->GetHotBorder());
	
	burningPos.x = TemperatureManager::GetInstance()->GetTemperature();
	burningPos.x = Util::Clamp(burningPos.x, 0.f, TemperatureManager::GetInstance()->GetBurningBorder());
}

void TemperatureUI::Draw()
{
	//ゲージの裏
	SimpleDrawer::DrawBox(position.x - frameSize.x / 2,
		position.y - frameSize.y / 2,
		position.x + 
		(TemperatureManager::GetInstance()->MAX_TEMPERATURE - TemperatureManager::GetInstance()->MIN_TEMPERATURE) * size.x +
		frameSize.x / 2,
		position.y + frameSize.y / 2, 0, frameColor, true);

	//ゲージ本体
	SimpleDrawer::DrawBox(position.x, 
		position.y - size.y / 2,
		position.x + burningPos.x * size.x - 
		TemperatureManager::GetInstance()->MIN_TEMPERATURE * size.x,
		position.y + size.y / 2, 0, burningColor, true);

	SimpleDrawer::DrawBox(position.x,
		position.y - size.y / 2,
		position.x + hotPos.x * size.x - 
		TemperatureManager::GetInstance()->MIN_TEMPERATURE * size.x,
		position.y + size.y / 2, 0, hotColor, true);

	SimpleDrawer::DrawBox(position.x,
		position.y - size.y / 2,
		position.x + coldPos.x * size.x - 
		TemperatureManager::GetInstance()->MIN_TEMPERATURE * size.x,
		position.y + size.y / 2, 0, coldColor, true);
}

void TemperatureUI::Save()
{
	Parameter::Begin("TemperatureUI");
	Parameter::Save("UI_X座標", position.x);
	Parameter::Save("UI_Y座標", position.y);
	Parameter::Save("UI_Xスケール", size.x);
	Parameter::Save("UI_Yスケール", size.y);
	Parameter::Save("フレームのサイズX", frameSize.x);
	Parameter::Save("フレームのサイズY", frameSize.y);
	Parameter::Save("背景色_R", frameColor.r);
	Parameter::Save("背景色_G", frameColor.g);
	Parameter::Save("背景色_B", frameColor.b);
	Parameter::Save("背景色_A", frameColor.a);
	Parameter::End();
}
