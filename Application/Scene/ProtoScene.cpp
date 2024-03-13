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
	EnemyManager::LoadResource();
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
	TemperatureManager::GetInstance()->Init();

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
			level.tower.Damage(1.f);
		}
		//蝋との当たり判定
		for (auto& group : WaxManager::GetInstance()->waxGroups)
		{
			for (auto& wax : group->waxs) {
				bool isCollision = ColPrimitive3D::CheckSphereToSphere(enemy.collider, wax->collider);

				if (isCollision && wax->isSolid == false) {
					//投げられてる蝋に当たった時は蝋固まり状態へ遷移
					if (wax->isGround == false) {
						enemy.ChangeState<EnemyWaxCoating>();
						enemy.trappedWax = wax.get();
						//enemyにダメージ
						enemy.DealDamage(WaxManager::GetInstance()->waxDamage);

					}
					//地面に付いた蝋に当たった時は蝋足止め状態へ遷移
					else {
						enemy.ChangeState<EnemySlow>();
						enemy.trappedWax = wax.get();
					}
				}
			}
		}
	}
	for (auto& fire : FireManager::GetInstance()->fires)
	{
		for (auto& group : WaxManager::GetInstance()->waxGroups)
		{
			for (auto& wax : group->waxs) {
				if (ColPrimitive3D::CheckSphereToSphere(wax->collider, fire.collider) && wax->isGround) {
					fire.SetIsAlive(false);
					wax->ChangeState<WaxIgnite>();
				}
			}
		}
	}
	
	player.Update();
	level.Update();

	//敵がロウを壊してから連鎖で壊れるため、敵の処理をしてからこの処理を行う
#pragma region ロウ同士の当たり判定
	//同一グループ内のロウが当たった時の処理
	for (auto& group : WaxManager::GetInstance()->waxGroups)
	{
		for (auto& wax1 : group->waxs) {
			for (auto& wax2 : group->waxs)
			{
				//同じ部分を指しているポインタなら同じものなのでスキップ
				if (wax1 == wax2)continue;

				bool isCollision = ColPrimitive3D::CheckSphereToSphere(wax1->collider, wax2->collider);

				//ぶつかっていて
				if (isCollision)
				{
					//燃えているものと通常の状態なら
					if (wax1->IsBurning() && wax2->IsNormal())
					{
						//燃えている状態へ遷移
						wax2->ChangeState<WaxIgnite>();
					}
					////どっちも液体なら
					//else if (wax1->isSolid == false && wax2->isSolid == false)
					//{
					//	

			//ここの処理は下のグループ移動の時に行う
					//	//固まる時間が長い方に優先
					//	if (wax1->solidTimer.nowTime_ > wax2->solidTimer.nowTime_)
					//	{
					//		wax1->solidTimer.nowTime_ = wax2->solidTimer.nowTime_;
					//	}
					//	else
					//	{
					//		wax2->solidTimer.nowTime_ = wax1->solidTimer.nowTime_;
					//	}
					//}
				}
			}
		}
	}
	//別グループ内のロウが当たった時の処理
	std::list<std::unique_ptr<WaxGroup>>* hoge = &WaxManager::GetInstance()->waxGroups;
	for (auto& group1 : *hoge)
	{
		for (auto& group2 : *hoge)
		{
			if (group1 == group2)continue;
			//nullチェック
			bool check = false;
			for (auto& wax : group1->waxs)
			{
				if (!wax) {
					check = true;
					group1->SetIsAlive(false);
					break;
				}
			}
			if (check) continue;
			for (auto& wax : group2->waxs)
			{
				if (!wax) {
					check = true;
					group2->SetIsAlive(false);
					break;
				}
			}
			if (check)continue;

			//こうしたい
			if (WaxManager::GetInstance()->CheckHitWaxGroups(group1, group2)) {
				//どれか一つがぶつかったなら、グループすべてが移動する
				group1->waxs.reserve(group1->waxs.size() + group2->waxs.size());
				std::move(group2->waxs.begin(), group2->waxs.end(), std::back_inserter(group1->waxs));
			}
		}
	}

#pragma endregion
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
