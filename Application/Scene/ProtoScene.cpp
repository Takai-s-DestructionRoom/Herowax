#include "ProtoScene.h"
#include "ResultScene.h"
#include "SceneManager.h"
#include "RInput.h"
#include "RImGui.h"
#include <Quaternion.h>

ProtoScene::ProtoScene()
{
	skydome = ModelObj(Model::Load("./Resources/Model/Skydome/Skydome.obj", "Skydome"));
	skydome.mTransform.scale = { 5, 5, 5 };
	skydome.mTransform.UpdateMatrix();

	ground = ModelObj(Model::Load("./Resources/Model/Ground/ground.obj", "Ground"));
	ground.mTransform.scale = { 5, 5, 5 };
	ground.mTransform.UpdateMatrix();

	camera.mViewProjection.mEye = { 0, 0, -50 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();
}

void ProtoScene::Init()
{
	Camera::sNowCamera = &camera;
	cameraDist = -20.f;
	cameraAngle = { 20.f,0.f };
	LightGroup::sNowLight = &light;
	tower.Init();
	player.Init();
}

void ProtoScene::Update()
{
	Vector3 cameraVec = { 0, 0, 1 };
	//カメラアングル適応
	cameraVec *= Quaternion::AngleAxis(Vector3(1, 0, 0).Cross(cameraVec), Util::AngleToRadian(cameraAngle.y));
	cameraVec *= Quaternion::AngleAxis(Vector3(0, 1, 0).Cross(cameraVec), Util::AngleToRadian(cameraAngle.x));
	//カメラの距離適応
	cameraVec *= cameraDist;

	//プレイヤーと一定の距離を保って着いていく
	camera.mViewProjection.mEye = player.GetPos() + cameraVec;
	//プレイヤーの方向いてくれる
	camera.mViewProjection.mTarget = player.GetPos();
	camera.mViewProjection.UpdateMatrix();

	//Rボタンでタワーにダメージ
	if (RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER))
	{
		tower.Damage(1);
	}

	tower.Update();
	player.Update();

	light.Update();

	skydome.TransferBuffer(camera.mViewProjection);
	ground.TransferBuffer(camera.mViewProjection);

	//スペースかAボタン押されたらリザルトシーンへ
	if (RInput::GetInstance()->GetKeyDown(DIK_SPACE) ||
		RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_START))
	{
		SceneManager::GetInstance()->Change<ResultScene>();
	}

#pragma region ImGui
	ImGui::SetNextWindowSize({ 300, 200 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	// カメラ //
	ImGui::Begin("Camera", NULL, window_flags);

	ImGui::Text("pos:%f,%f,%f",
		camera.mViewProjection.mEye.x, camera.mViewProjection.mEye.y, camera.mViewProjection.mEye.z);
	ImGui::SliderFloat("dist:%f", &cameraDist, -50.f, 0.f);
	ImGui::SliderAngle("angleX:%f", &cameraAngle.x);
	ImGui::SliderAngle("angleY:%f", &cameraAngle.y);

	ImGui::End();
#pragma endregion
}

void ProtoScene::Draw()
{
	skydome.Draw();
	ground.Draw();
	tower.Draw();
	player.Draw();
}
