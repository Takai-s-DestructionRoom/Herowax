#include "WaveUI.h"
#include "EnemyManager.h"
#include "RImGui.h"

void WaveUI::LoadResource()
{
	atText = TextureManager::Load("./Resources/UI/at.png", "at");
}

void WaveUI::Init()
{
	numDrawer.Init(2,"waveUI_numDrawer");
}

void WaveUI::Update()
{
	numDrawer.SetNum((int32_t)EnemyManager::GetInstance()->enemys.size());
	numDrawer.Imgui();
	numDrawer.Update();

	if (RImGui::showImGui) {
		ImGui::Begin("waveUI_numDrawer2");
		ImGui::DragFloat3("atText:pos", &atText.mTransform.position.x);
		ImGui::DragFloat3("atText:scale", &atText.mTransform.scale.x);
		ImGui::End();
	}

	atText.mTransform.UpdateMatrix();
	atText.TransferBuffer();
}

void WaveUI::Draw()
{
	numDrawer.Draw();
	atText.Draw();
}
