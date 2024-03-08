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
#include "FireManager.h"

ProtoScene::ProtoScene()
{
	skydome = ModelObj(Model::Load("./Resources/Model/Skydome/Skydome.obj", "Skydome"));
	skydome.mTransform.scale = { 5, 5, 5 };
	skydome.mTransform.UpdateMatrix();

	camera.mViewProjection.mEye = { 0, 0, -50 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();

	TemperatureUI::LoadResource();
	InstantDrawer::PreCreate();

	level.Load();
}

void ProtoScene::Init()
{
	Camera::sNowCamera = &camera;
	cameraDist = -20.f;
	cameraAngle = { Util::AngleToRadian(20.f),0.f };
	LightGroup::sNowLight = &light;

	player.Init();

	ParticleManager::GetInstance()->Init();

	WaxManager::GetInstance()->Init();
	FireManager::GetInstance()->Init();

	//とりあえず最初のステージを設定しておく
	level.Extract("test");
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

	//ここに無限に当たり判定増やしていくの嫌なのであとで何か作ります
	//クソ手抜き当たり判定
	for (auto& enemy : EnemyManager::GetInstance()->enemys)
	{
		//タワーとの当たり判定
		if (ColPrimitive3D::CheckSphereToSphere(enemy.collider, level.tower.collider)) {
			enemy.SetIsAlive(false);
			level.tower.Damage(1);
		}
		//蝋との当たり判定
		for (auto& wax : WaxManager::GetInstance()->waxs)
		{
			//液体の蝋に当たってたら
			if (ColPrimitive3D::CheckSphereToSphere(enemy.collider, wax->collider) &&
				wax->isSolid == false)
			{
				enemy.SetIsSlow(true);	//蝋に設定された減速率をもとに敵を足止め
			}
			else
			{
				enemy.SetIsSlow(false);
			}
		}
	}
	for (auto& fire : FireManager::GetInstance()->fires)
	{
		for (auto& wax : WaxManager::GetInstance()->waxs)
		{
			if (ColPrimitive3D::CheckSphereToSphere(wax->collider, fire.collider)) {
				fire.SetIsAlive(false);
				wax->ChangeState(new WaxIgnite());
			}
		}
	}
	for (auto& wax1 : WaxManager::GetInstance()->waxs)
	{
		//燃えているなら
		if (wax1->IsBurning())
		{
			for (auto& wax2 : WaxManager::GetInstance()->waxs)
			{
				//ぶつかっていて通常の状態なら
				if (ColPrimitive3D::CheckSphereToSphere(wax1->collider, wax2->collider) &&
					wax2->IsNormal())
				{
					//燃えている状態へ遷移
					wax2->ChangeState(new WaxIgnite());
					//燃えたときに、すでに燃えている蝋の数に応じてボーナス
					TemperatureManager::GetInstance()->TemperaturePlus(
						WaxManager::GetInstance()->GetCalcHeatBonus());
				}
			}
		}
	}

	player.Update();
	level.Update();

	WaxManager::GetInstance()->Update();
	FireManager::GetInstance()->Update();
	TemperatureManager::GetInstance()->Update();
	ParticleManager::GetInstance()->Update();

	light.Update();

	skydome.TransferBuffer(camera.mViewProjection);

	//F6かメニューボタン押されたらリザルトシーンへ
	if (RInput::GetInstance()->GetKeyDown(DIK_F6) ||
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
	ImGui::SliderFloat("カメラ距離:%f", &cameraDist, -500.f, 0.f);
	ImGui::SliderAngle("カメラアングルX:%f", &cameraAngle.x);
	ImGui::SliderAngle("カメラアングルY:%f", &cameraAngle.y);

	ImGui::End();
#pragma endregion
}

void ProtoScene::Draw()
{
	ParticleManager::GetInstance()->Draw();
	skydome.Draw();
	WaxManager::GetInstance()->Draw();
	FireManager::GetInstance()->Draw();
	TemperatureManager::GetInstance()->Draw();
	player.Draw();

	level.Draw();

	//更新
	InstantDrawer::AllUpdate();
	InstantDrawer::AllDraw2D();
}
