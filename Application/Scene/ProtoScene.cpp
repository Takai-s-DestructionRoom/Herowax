#include "ProtoScene.h"
#include "RInput.h"
#include "RImGui.h"
#include <Quaternion.h>

ProtoScene::ProtoScene()
{
	skydome = ModelObj(Model::Load("./Resources/Model/Skydome/Skydome.obj", "Skydome"));
	skydome.mTransform.scale = { 5, 5, 5 };
	skydome.mTransform.UpdateMatrix();

	camera.mViewProjection.mEye = { 0, 0, -50 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();
}

void ProtoScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
	player.Init();
}

void ProtoScene::Update()
{
	//テストGUI
	{
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;

		// プレイヤー //
		ImGui::Begin("Player", NULL, window_flags);

		ImGui::Text("pos:%f,%f,%f", player.GetPos().x, player.GetPos().y, player.GetPos().z);

		if (ImGui::Button("Reset")) {
			player.SetPos({ 0, 0, 0 });
		}

		ImGui::End();

		// カメラ //
		ImGui::Begin("Camera", NULL, window_flags);

		ImGui::Text("pos:%f,%f,%f",
			camera.mViewProjection.mEye.x, camera.mViewProjection.mEye.y, camera.mViewProjection.mEye.z);

		ImGui::End();
	}

	Vector3 cameraVec = { 0, 0, 1 };
	//cameraVec *= Quaternion::AngleAxis(Vector3(0, 1, 0).Cross(cameraVec), Util::AngleToRadian(20));
	//cameraVec *= -20.0f;

	//camera.mViewProjection.mEye = player.GetPos() + cameraVec;
	//camera.mViewProjection.mTarget = {0,0,0};
	camera.mViewProjection.UpdateMatrix();

	player.Update();

	light.Update();

	skydome.TransferBuffer(camera.mViewProjection);
}

void ProtoScene::Draw()
{
	skydome.Draw();
	player.Draw();
}
