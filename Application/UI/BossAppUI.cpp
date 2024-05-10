#include "BossAppUI.h"
#include "RImGui.h"
#include "Parameter.h"

void BossAppUI::Init()
{
	atText = TextureManager::Load("./Resources/UI/at.png", "at");
	bossAppText = TextureManager::Load("./Resources/UI/bossAppText.png", "bossAppText");

	TimerUI::Init();

	std::map<std::string, std::string> extract = Parameter::Extract("BossTimerUI");
	atPlusPos = Parameter::GetVector3Data(extract,"「あと」の位置", atPlusPos);
	atPlusScale = Parameter::GetVector3Data(extract,"「あと」の大きさ", atPlusScale);
	bossAppPlusPos = Parameter::GetVector3Data(extract,"「ボスが..」の位置", bossAppPlusPos);
	bossAppPlusScale = Parameter::GetVector3Data(extract,"「ボスが..」の大きさ", bossAppPlusScale);

	basePos = Parameter::GetVector3Data(extract, "位置", basePos);
	baseScale = Parameter::GetVector3Data(extract, "大きさ", baseScale);
	interX = Parameter::GetParam(extract, "間隔", interX);
}

void BossAppUI::Update()
{
	atText.mTransform.position = basePos + atPlusPos;
	bossAppText.mTransform.position = basePos + bossAppPlusPos;

	atText.mTransform.scale = atPlusScale;
	bossAppText.mTransform.scale = bossAppPlusScale;

	atText.mTransform.UpdateMatrix();
	atText.TransferBuffer();

	bossAppText.mTransform.UpdateMatrix();
	bossAppText.TransferBuffer();

	TimerUI::Update();
}

void BossAppUI::Draw()
{
	TimerUI::Draw();
	atText.Draw();

	bossAppText.Draw();
}

void BossAppUI::Imgui(const std::string& title)
{
	if (RImGui::showImGui) {
		ImGui::Begin(title.c_str());
		ImGui::Text("minute:%d", minute);
		ImGui::Text("tenSecond:%d", tenSecond);
		ImGui::Text("oneSecond:%d", oneSecond);
		ImGui::DragFloat("タイマーの最大の時間", &timer.maxTime_);
		ImGui::DragFloat3("位置", &basePos.x);
		ImGui::DragFloat3("大きさ", &baseScale.x, 0.1f);

		ImGui::DragFloat3("「あと」の位置", &atPlusPos.x);
		ImGui::DragFloat3("「あと」の大きさ", &atPlusScale.x, 0.1f);
		ImGui::DragFloat3("「ボスが..」の位置", &bossAppPlusPos.x);
		ImGui::DragFloat3("「ボスが..」の大きさ", &bossAppPlusScale.x, 0.1f);
		
		ImGui::DragFloat("間隔", &interX);

		if (ImGui::Button("初期化して開始")) {
			Start();
		}
		if (ImGui::Button("途中から開始")) {
			ReStart();
		}
		if (ImGui::Button("停止")) {
			Stop();
		}

		if (ImGui::Button("セーブ")) {
			Parameter::Begin("BossTimerUI");
			Parameter::SaveVector3("位置", basePos);
			Parameter::SaveVector3("大きさ", baseScale);
			Parameter::Save("間隔", interX);
			Parameter::SaveVector3("「あと」の位置", atPlusPos);
			Parameter::SaveVector3("「あと」の大きさ", atPlusScale);
			Parameter::SaveVector3("「ボスが..」の位置", bossAppPlusPos);
			Parameter::SaveVector3("「ボスが..」の大きさ", bossAppPlusScale);

			Parameter::End();
		}

		ImGui::End();
	}
}
