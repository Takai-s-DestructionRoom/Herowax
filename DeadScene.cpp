#include "DeadScene.h"
#include "RInput.h"
#include "SceneManager.h"
#include "ProtoScene.h"
#include "RImGui.h"

DeadScene::DeadScene()
{

}

void DeadScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;

	skydome = ModelObj(Model::Load("./Resources/Model/bg/bg.obj", "bg"));
	skydome.mTransform.scale = { 1.5f, 1.5f, 1.5f };
	skydome.mTransform.UpdateMatrix();

	obj = ModelObj(Model::Load("./Resources/Model/cube.obj", "cube", true));
}

void DeadScene::Update()
{
	//F6かメニューボタン押されたらプロトシーンへ
	if (RInput::GetInstance()->GetKeyDown(DIK_F6) ||
		RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_START))
	{
		SceneManager::GetInstance()->Change<ProtoScene>();
	}

	ImGui::SetNextWindowSize({ 400, 200 }, ImGuiCond_FirstUseEver);
	ImGui::Begin("ライト");
	ImGui::DragFloat3("atten", & atten.x,0.1f);
	ImGui::DragFloat4("color", &color.r,0.1f);
	ImGui::DragFloat3("dir", &dir.x,0.1f);
	ImGui::DragFloat3("factorAngle", &factorAngle.x,0.1f);
	ImGui::DragFloat3("pos", &pos.x,0.1f);
	ImGui::End();

	light.SetSpotLightActive(lightindex, 1);
	light.SetSpotLightAtten(lightindex, atten);
	light.SetSpotLightColor(lightindex,color);
	light.SetSpotLightDirection(lightindex, dir);
	light.SetSpotLightFactorAngle(lightindex, factorAngle);
	light.SetSpotLightPos(lightindex,pos);
	
	camera.Update();

	light.Update();

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);

	skydome.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void DeadScene::Draw()
{
	skydome.Draw();
	obj.Draw();
}
