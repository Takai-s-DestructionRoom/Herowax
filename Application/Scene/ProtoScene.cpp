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
#include "Parameter.h"
#include "SpawnOrderData.h"

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

	std::map<std::string, std::string> extract = Parameter::Extract("Camera");
	cameraDist = Parameter::GetParam(extract,"カメラ距離", -20.f);
	cameraAngle.x = Parameter::GetParam(extract,"カメラアングルX", Util::AngleToRadian(20.f));
	cameraAngle.y = Parameter::GetParam(extract,"カメラアングルY", 0.f);
	cameraSpeed = Parameter::GetParam(extract,"カメラの移動速度", 0.01f);

	LightGroup::sNowLight = &light;

	player.Init();

	ParticleManager::GetInstance()->Init();

	WaxManager::GetInstance()->Init();
	FireManager::GetInstance()->Init();
	TemperatureManager::GetInstance()->Init();

	EnemyManager::GetInstance()->SetGround(&level.ground);

	//とりあえず最初のステージを設定しておく
	level.Extract("test");
}

void ProtoScene::Update()
{
	InstantDrawer::DrawInit();

	Vector2 stick = RInput::GetInstance()->GetRStick(false, true);

	if (stick.LengthSq() > 0.0f) {
		float moveSpeed = cameraSpeed;

		if (!std::signbit(stick.x)) {
			moveSpeed *= -1;
		}
		cameraAngle.y += moveSpeed;
	}

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
		if (ColPrimitive3D::CheckSphereToSphere(enemy->collider, level.tower.collider)) {
			enemy->SetDeath();
			Vector3 vec = level.tower.GetPos() - enemy->GetPos();
			level.tower.Damage(1.f,vec);
		}
		//蝋との当たり判定
		for (auto& group : WaxManager::GetInstance()->waxGroups)
		{
			bool isHitEnemy = false;
			for (auto& wax : group->waxs) {
				bool isCollision = ColPrimitive3D::CheckSphereToSphere(enemy->collider, wax->collider);

				if (isCollision && wax->isSolid == false) {
					//投げられてる蝋に当たった時は蝋固まり状態へ遷移
					if (wax->isGround == false) {
						enemy->ChangeState<EnemyWaxCoating>();
						
						//enemyにダメージ
						Vector3 knockVec = player.atkVec;
						knockVec.y = 0;
						enemy->DealDamage(WaxManager::GetInstance()->waxDamage,
							knockVec, &player.obj);

						//お試し実装:自分が攻撃を当てた相手が自分を追いかけてくる
						if (player.GetTauntMode()) {
							enemy->SetTarget(&player.obj);
						}
					}
					else
					{
						isHitEnemy = true;
					}
				}
			}
			if (isHitEnemy) {
				group->trapEnemys.push_back(enemy.get());
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
	
	//敵がロウを壊してから連鎖で壊れるため、敵の処理をしてからこの処理を行う
#pragma region ロウ同士の当たり判定
	std::list<std::unique_ptr<WaxGroup>>* wGroups = &WaxManager::GetInstance()->waxGroups;

	//同一グループ内のロウが当たった時の処理
	for (auto& group1 : *wGroups)
	{
		for (auto& group2 : *wGroups)
		{
			for (auto& wax1 : group1->waxs) {
				for (auto& wax2 : group2->waxs)
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
					}
				}
			}
		}
	}
	

	//別グループ内のロウが当たった時の処理
	for (auto& group1 : *wGroups)
	{
		for (auto& group2 : *wGroups)
		{
			if (group1 == group2)continue;
		
			//こうしたい
			if (WaxManager::GetInstance()->CheckHitWaxGroups(group1, group2)) {
				//どれか一つがぶつかったなら、グループすべてが移動する
				for (auto& enemy : EnemyManager::GetInstance()->enemys)
				{
					//エネミーが保持しているポインタと同じ位置を指そうとしているなら変える
					if (enemy->trappedWaxGroup == group2.get()) {
						enemy->trappedWaxGroup = group1.get();
					}
				}
				group1->waxs.splice(group1->waxs.end(), std::move(group2->waxs));
				group1->SetSameSolidTime();
			}
		}
	}

#pragma endregion
	WaxManager::GetInstance()->Update();
	FireManager::GetInstance()->Update();
	TemperatureManager::GetInstance()->Update();
	ParticleManager::GetInstance()->Update();

	level.Update();

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
	ImGui::SliderFloat("カメラの移動速度", &cameraSpeed,0.0f,0.5f);
	
	if (ImGui::Button("セーブ")) {
		Parameter::Begin("Camera");
		Parameter::Save("カメラ距離", cameraDist);
		Parameter::Save("カメラアングルX", cameraAngle.x);
		Parameter::Save("カメラアングルY", cameraAngle.y);
		Parameter::Save("カメラの移動速度", cameraSpeed);
		Parameter::End();
	}

	ImGui::End();

	SpawnDataLoader::OrderCreateGUI();

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
