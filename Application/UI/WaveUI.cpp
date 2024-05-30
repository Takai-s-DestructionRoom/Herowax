#include "WaveUI.h"
#include "EnemyManager.h"
#include "RImGui.h"
#include "Parameter.h"
#include "Wave.h"

void WaveUI::LoadResource()
{
	atText = TextureManager::Load("./Resources/UI/at.png", "at");
	nextwave = TextureManager::Load("./Resources/UI/nextwave.png", "nextwave");
	tai = TextureManager::Load("./Resources/UI/tai.png", "tai");
	enemyAllKillText = TextureManager::Load("./Resources/UI/enemyAllKillText.png", "enemyAllKillText");
	waveBase = TextureManager::Load("./Resources/UI/wave.png", "waveBase");
}

void WaveUI::Init()
{
	numDrawer.Init(2,"waveUI_numDrawer");
	nowwaveDrawer.Init(1,"waveUI_nowwaveDrawer");
	maxwaveDrawer.Init(1,"waveUI_maxwaveDrawer");

	std::map<std::string, std::string> extract =  Parameter::Extract("waveUI");
	atText.mTransform.position = Parameter::GetVector3Data(extract,"atText_pos", atText.mTransform.position);
	atText.mTransform.scale = Parameter::GetVector3Data(extract,"atText_scale", atText.mTransform.scale);
	nextwave.mTransform.position = Parameter::GetVector3Data(extract,"nextwave_pos", nextwave.mTransform.position);
	nextwave.mTransform.scale = Parameter::GetVector3Data(extract,"nextwave_scale", nextwave.mTransform.scale);
	tai.mTransform.position = Parameter::GetVector3Data(extract,"tai_pos", tai.mTransform.position);
	tai.mTransform.scale = Parameter::GetVector3Data(extract,"tai_scale", tai.mTransform.scale);
	enemyAllKillText.mTransform.position = Parameter::GetVector3Data(extract,"enemyAllKillText_pos", enemyAllKillText.mTransform.position);
	enemyAllKillText.mTransform.scale = Parameter::GetVector3Data(extract,"enemyAllKillText_scale", enemyAllKillText.mTransform.scale);
	waveBase.mTransform.position = Parameter::GetVector3Data(extract,"waveBase_pos", waveBase.mTransform.position);
	waveBase.mTransform.scale = Parameter::GetVector3Data(extract,"waveBase_scale", waveBase.mTransform.scale);
}

void WaveUI::Update()
{
	numDrawer.SetNum((int32_t)EnemyManager::GetInstance()->enemys.size());
	numDrawer.Imgui();
	numDrawer.Update();

	nowwaveDrawer.SetNum(WaveManager::Get()->GetNowWave() + 1);
	nowwaveDrawer.Imgui();
	nowwaveDrawer.Update();

	maxwaveDrawer.SetNum(WaveManager::Get()->GetMaxWave());
	maxwaveDrawer.Imgui();
	maxwaveDrawer.Update();

	if (RImGui::showImGui) {
		ImGui::Begin("waveUI_numDrawer2");
		ImGui::DragFloat3("atText:pos", &atText.mTransform.position.x);
		ImGui::DragFloat3("atText:scale", &atText.mTransform.scale.x);
		ImGui::DragFloat3("nextwave:pos", &nextwave.mTransform.position.x);
		ImGui::DragFloat3("nextwave:scale", &nextwave.mTransform.scale.x);
		ImGui::DragFloat3("tai:pos", &tai.mTransform.position.x);
		ImGui::DragFloat3("tai:scale", &tai.mTransform.scale.x);
		ImGui::DragFloat3("enemyAllKillText_pos", &enemyAllKillText.mTransform.position.x);
		ImGui::DragFloat3("enemyAllKillText_scale", &enemyAllKillText.mTransform.scale.x);
		ImGui::DragFloat3("waveBase_pos", &waveBase.mTransform.position.x);
		ImGui::DragFloat3("waveBase_scale", &waveBase.mTransform.scale.x,0.1f);

		if (ImGui::Button("セーブ")) {
			Parameter::Begin("waveUI");
			Parameter::SaveVector3("atText_pos", atText.mTransform.position);
			Parameter::SaveVector3("atText_scale", atText.mTransform.scale);
			Parameter::SaveVector3("nextwave_pos", nextwave.mTransform.position);
			Parameter::SaveVector3("nextwave_scale", nextwave.mTransform.scale);
			Parameter::SaveVector3("tai_pos", tai.mTransform.position);
			Parameter::SaveVector3("tai_scale", tai.mTransform.scale);
			Parameter::SaveVector3("enemyAllKillText_pos", enemyAllKillText.mTransform.position);
			Parameter::SaveVector3("enemyAllKillText_scale", enemyAllKillText.mTransform.scale);
			Parameter::SaveVector3("waveBase_pos", waveBase.mTransform.position);
			Parameter::SaveVector3("waveBase_scale", waveBase.mTransform.scale);
			Parameter::End();
		}

		ImGui::End();
	}

	atText.mTransform.UpdateMatrix();
	atText.TransferBuffer();
	
	nextwave.mTransform.UpdateMatrix();
	nextwave.TransferBuffer();
	
	tai.mTransform.UpdateMatrix();
	tai.TransferBuffer();
	
	enemyAllKillText.mTransform.UpdateMatrix();
	enemyAllKillText.TransferBuffer();

	waveBase.mTransform.UpdateMatrix();
	waveBase.TransferBuffer();

	timerUI.Imgui("WaveUI_Timer");
	timerUI.SetMaxTime(WaveManager::Get()->GetWaitTimer().maxTime_);
	timerUI.Update();
}

void WaveUI::Draw()
{
	numDrawer.Draw();
	atText.Draw();
	tai.Draw();
	enemyAllKillText.Draw();
	waveBase.Draw();
	nowwaveDrawer.Draw();
	maxwaveDrawer.Draw();
}
