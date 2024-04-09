#include "ParticleEditorScene.h"
#include "ParticleEditor.h"
#include "RImGui.h"
#include "ParticleManager.h"

void ParticleEditorScene::Init()
{
	ParticleManager::GetInstance()->Init();
	ParticleManager::GetInstance()->SetPlayerPos({ 0,0,0 });

	camera.mViewProjection.mEye = { 0, 0, -10 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();

	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;

	drawEmitter = ModelObj(Model::Load("./Resources/Model/Cube.obj","Cube"));
	drawEmitter.mTransform.scale = { 1,1,1 };
}

void ParticleEditorScene::Update()
{
	camera.Update();
	light.Update();
	
	drawEmitter.mTransform.position = emitPos;
	drawEmitter.mTransform.UpdateMatrix();
	drawEmitter.TransferBuffer(camera.mViewProjection);
	
	ParticleEditor::OrderCreateGUI();
	roopTimer.Update();

	if (isAutoCreate) {
		if (!roopTimer.GetRun()) {
			roopTimer.Start();

			if (loadPartName != "") {
				SimplePData pData = ParticleEditor::LoadSimple(loadPartName);
				ParticleManager::GetInstance()->AddSimple(emitPos, pData);
			}
		}
	}

	ParticleManager::GetInstance()->Update();

	ImGui::SetNextWindowSize({ 400, 400 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("パーティクル生成GUI", NULL, window_flags);
	ImGui::InputText("読み込むパーティクル名", &loadPartName);
	ImGui::DragFloat3("生成位置", &emitPos.x);
	ImGui::Checkbox("無限に生成するか", &isAutoCreate);
	ImGui::InputFloat("無限生成の間隔", &roopTimer.maxTime_,0.1f);
	ImGui::End();
}

void ParticleEditorScene::Draw()
{
	drawEmitter.Draw();
	ParticleManager::GetInstance()->Draw();
}

void ParticleEditorScene::Finalize()
{
}
