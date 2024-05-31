#include "ControlUI.h"
#include "InstantDrawer.h"
#include "RImGui.h"
#include "Parameter.h"

void ControlUI::LoadResource()
{
	TextureManager::Load("./Resources/UI/L_Stick.png", "L_Stick");
	TextureManager::Load("./Resources/UI/LT.png", "LT");
	TextureManager::Load("./Resources/UI/R_Stick.png", "R_Stick");
	TextureManager::Load("./Resources/UI/RT.png", "RT");
	TextureManager::Load("./Resources/UI/Stick_shaft.png", "Stick_shaft");
	TextureManager::Load("./Resources/UI/A_normal.png", "A_normal");
	TextureManager::Load("./Resources/UI/X_normal.png", "X_normal");

	TextureManager::Load("./Resources/UI/cameraText.png", "cameraText");
	TextureManager::Load("./Resources/UI/collectText.png", "collectText");
	TextureManager::Load("./Resources/UI/moveText.png", "moveText");
	TextureManager::Load("./Resources/UI/shotText.png", "shotText");
	TextureManager::Load("./Resources/UI/guardText.png", "guardText");
	TextureManager::Load("./Resources/UI/avoidText.png", "avoidText");
}

ControlUI::ControlUI()
{

}

void ControlUI::Init()
{
	auto extract = Parameter::Extract("UI配置");

	for (int32_t i = 0; i < uiOnces.size(); i++)
	{
		std::string text1 = "uiPosX" + std::to_string(i);
		std::string text2 = "uiPosY" + std::to_string(i);
		std::string text3 = "uiSizeX" + std::to_string(i);
		std::string text4 = "uiSizeZ" + std::to_string(i);
		uiOnces[i].position.x = Parameter::GetParam(extract, text1, 0);
		uiOnces[i].position.y = Parameter::GetParam(extract, text2, 0);
		uiOnces[i].size.x = Parameter::GetParam(extract, text3, 0);
		uiOnces[i].size.y = Parameter::GetParam(extract, text4, 0);
	}

	uiOnces[0].load_file = "L_Stick";
	uiOnces[1].load_file = "moveText";
	uiOnces[2].load_file = "R_Stick";
	uiOnces[3].load_file = "cameraText";
	uiOnces[4].load_file = "RT";
	uiOnces[5].load_file = "shotText";
	uiOnces[6].load_file = "LT";
	uiOnces[7].load_file = "collectText";
	uiOnces[8].load_file = "A_normal";
	uiOnces[9].load_file = "guardText";
	uiOnces[10].load_file = "X_normal";
	uiOnces[11].load_file = "avoidText";

	uiOnces[0].alpha = 0.f;
	uiOnces[1].alpha = 0.f;
	uiOnces[2].alpha = 0.f;
	uiOnces[3].alpha = 0.f;
	uiOnces[8].alpha = 0.f;
	uiOnces[11].alpha = 0.f;
}

void ControlUI::Update()
{
	if (RImGui::showImGui)
	{
		ImGui::SetNextWindowSize({ 400, 200 }, ImGuiCond_FirstUseEver);

		// デバッグモード //
		ImGui::Begin("操作説明UI");

		for (int32_t i = 0; i < uiOnces.size(); i++)
		{
			std::string text1 = "ui位置" + std::to_string(i);
			std::string text2 = "uiサイズ" + std::to_string(i);
			ImGui::Text(uiOnces[i].load_file.c_str());
			ImGui::DragFloat2(text1.c_str(), &uiOnces[i].position.x);
			ImGui::DragFloat2(text2.c_str(), &uiOnces[i].size.x);
		}

		if (ImGui::Button("セーブ")) {
			Parameter::Begin("UI配置");
			for (int32_t i = 0; i < uiOnces.size(); i++)
			{
				std::string text1 = "uiPosX" + std::to_string(i);
				std::string text2 = "uiPosY" + std::to_string(i);
				std::string text3 = "uiSizeX" + std::to_string(i);
				std::string text4 = "uiSizeZ" + std::to_string(i);
				Parameter::Save(text1, uiOnces[i].position.x);
				Parameter::Save(text2, uiOnces[i].position.y);
				Parameter::Save(text3, uiOnces[i].size.x);
				Parameter::Save(text4, uiOnces[i].size.y);
			}
			Parameter::End();
		}

		ImGui::End();
	}
}

void ControlUI::Draw()
{
	for (int32_t i = 0; i < uiOnces.size(); i++)
	{
		uiOnces[i].Draw();
	}
}

void UIOnce::Draw()
{
	InstantDrawer::DrawGraph(position.x, position.y, size.x, size.y, 0, load_file, { 1,1,1,alpha });
}
