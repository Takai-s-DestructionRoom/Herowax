#include "ProtoScene.h"
#include "ResultScene.h"
#include "SceneManager.h"
#include "WaxManager.h"
#include "ParticleManager.h"
#include "RInput.h"
#include "RImGui.h"
#include <Quaternion.h>
#include "ColPrimitive3D.h"
#include "InstantDrawer.h"
#include "Temperature.h"

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

	TemperatureUI::LoadResource();
	InstantDrawer::PreCreate();
}

void ProtoScene::Init()
{
	Camera::sNowCamera = &camera;
	cameraDist = -20.f;
	cameraAngle = { Util::AngleToRadian(20.f),0.f };
	LightGroup::sNowLight = &light;

	ParticleManager::GetInstance()->Init();

	tower.Init();
	player.Init();

	EnemyManager::GetInstance()->Init();
	EnemyManager::GetInstance()->SetTarget(&tower.obj);

	enemySpawner.Init();
	enemySpawner.SetPos({30,0,0});

	WaxManager::GetInstance()->Init();
}

void ProtoScene::Update()
{
	InstantDrawer::DrawInit();

	Vector3 cameraVec = { 0, 0, 1 };
	//カメラアングル適応
	cameraVec *= Quaternion::AngleAxis(Vector3(1, 0, 0).Cross(cameraVec), cameraAngle.y);
	cameraVec *= Quaternion::AngleAxis(Vector3(0, 1, 0).Cross(cameraVec), cameraAngle.x);
	//カメラの距離適応
	cameraVec *= cameraDist;

	//プレイヤーと一定の距離を保って着いていく
	camera.mViewProjection.mEye = player.GetPos() + cameraVec;
	//プレイヤーの方向いてくれる
	camera.mViewProjection.mTarget = player.GetPos();
	camera.mViewProjection.UpdateMatrix();

	//Rキーでタワーにダメージ
	if (RInput::GetInstance()->GetKeyDown(DIK_R))
	{
		tower.Damage(1);
	}

	//3秒ごとに1回敵を出現させる
	enemySpawner.Update();
	EnemyManager::GetInstance()->Update();
	//クソ手抜き当たり判定
	for (auto& enemy : EnemyManager::GetInstance()->enemys)
	{
		if (ColPrimitive3D::CheckSphereToSphere(enemy.collider, tower.collider)) {
			enemy.SetIsAlive(false);
			tower.Damage(1);
		}
	}
	tower.Update();
	player.Update();

	WaxManager::GetInstance()->Update();
	TemperatureManager::GetInstance()->Update();
	ParticleManager::GetInstance()->Update();

	light.Update();

	skydome.TransferBuffer(camera.mViewProjection);
	ground.TransferBuffer(camera.mViewProjection);

	//エンターかメニューボタン押されたらリザルトシーンへ
	if (RInput::GetInstance()->GetKeyDown(DIK_RETURN) ||
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

	ImGui::Text("座標:%f,%f,%f",
		camera.mViewProjection.mEye.x, camera.mViewProjection.mEye.y, camera.mViewProjection.mEye.z);
	ImGui::SliderFloat("カメラ距離:%f", &cameraDist, -50.f, 0.f);
	ImGui::SliderAngle("カメラアングルX:%f", &cameraAngle.x);
	ImGui::SliderAngle("カメラアングルY:%f", &cameraAngle.y);

	ImGui::End();
#pragma endregion
}

void ProtoScene::Draw()
{
	ParticleManager::GetInstance()->Draw();
	EnemyManager::GetInstance()->Draw();
	enemySpawner.Draw();
	skydome.Draw();
	ground.Draw();
	tower.Draw();
	WaxManager::GetInstance()->Draw();
	TemperatureManager::GetInstance()->Draw();
	player.Draw();

	//更新
	InstantDrawer::AllUpdate();
	InstantDrawer::AllDraw2D();
}
