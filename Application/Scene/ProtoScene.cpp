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
#include "Minimap.h"

ProtoScene::ProtoScene()
{
	TextureManager::Load("./Resources/Brush.png", "brush");

	skydome = ModelObj(Model::Load("./Resources/Model/Skydome/Skydome.obj", "Skydome"));
	skydome.mTransform.scale = { 5, 5, 5 };
	skydome.mTransform.UpdateMatrix();

	camera.mViewProjection.mEye = { 0, 0, -50 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();

	TemperatureUI::LoadResource();
	EnemyManager::LoadResource();
	InstantDrawer::PreCreate();

	Level::Get()->Load();
	
	wave.Load();

	EggUI::LoadResource();
}

void ProtoScene::Init()
{
	Camera::sNowCamera = &camera;
	Camera::sMinimapCamera = &minimapCamera;

	std::map<std::string, std::string> extract = Parameter::Extract("Camera");
	cameraDist = Parameter::GetParam(extract,"カメラ距離", -20.f);
	cameraAngle.x = Parameter::GetParam(extract,"カメラアングルX", Util::AngleToRadian(20.f));
	cameraAngle.y = Parameter::GetParam(extract,"カメラアングルY", 0.f);
	cameraSpeed = Parameter::GetParam(extract,"カメラの移動速度", 0.01f);
	mmCameraDist = Parameter::GetParam(extract,"ミニマップ用カメラ距離", -250.f);

	LightGroup::sNowLight = &light;

	player.Init();

	ParticleManager::GetInstance()->Init();

	WaxManager::GetInstance()->Init();
	FireManager::GetInstance()->Init();
	TemperatureManager::GetInstance()->Init();

	//とりあえず最初のステージを設定しておく
	Level::Get()->Extract("test");

	EnemyManager::GetInstance()->SetGround(&Level::Get()->ground);
	EnemyManager::GetInstance()->SetTarget(&player.obj);

	eggUI.Init();
	eggUI.SetTower(&Level::Get()->tower);


	//nest.Init();

	//nest.SetGround(Level::Get()->ground);

	Minimap::GetInstance()->Init();

	extract = Parameter::Extract("DebugBool");
	Util::debugBool = Parameter::GetParam(extract, "debugBool", false);
}

void ProtoScene::Update()
{
	//初期化周り
	InstantDrawer::DrawInit();

	//EnemyManager::GetInstance()->Delete();
	//WaxManager::GetInstance()->Delete();

	Vector2 stick = RInput::GetInstance()->GetRStick(false, true);

	if (stick.LengthSq() > 0.0f) {
		float moveSpeed = cameraSpeed;

		if (abs(stick.x) > 0.3f) {
			cameraAngle.y += moveSpeed * -stick.x;
		}
		if (abs(stick.y) > 0.3f) {
			cameraAngle.x += moveSpeed * stick.y;
		}
	}
	cameraAngle.x = Util::Clamp(cameraAngle.x, 0.f,Util::AngleToRadian(89.f));
	//cameraAngle.x += moveSpeed;

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

	MinimapCameraUpdate();

	//ここに無限に当たり判定増やしていくの嫌なのであとで何か作ります
	//クソ手抜き当たり判定
	
	for (auto& enemy : EnemyManager::GetInstance()->enemys)
	{
		//タワーとの当たり判定
		if (ColPrimitive3D::CheckSphereToSphere(enemy->collider, 
			Level::Get()->tower.collider)) {
			enemy->SetDeath();
			Vector3 vec = Level::Get()->tower.GetPos() - enemy->GetPos();
			Level::Get()->tower.Damage(1.f,vec);
		}
		//プレイヤーとの当たり判定
		//特定範囲内に入ったら敵を攻撃状態へ遷移
		if (ColPrimitive3D::CheckSphereToSphere(enemy->attackHitCollider,
			player.attackHitCollider)) {
			if (enemy->GetAttackState() == "NonAttack") {
				enemy->ChangeAttackState<EnemyFindState>();
			}
		}
	}

	for (auto& enemy : EnemyManager::GetInstance()->enemys)
	{
		//蝋と当たり判定をする前に、足盗られは毎フレーム解除判定を行う
		if (enemy->GetState() == "Slow") {
			enemy->ChangeState<EnemyNormal>();
		}
	}

	//蝋との当たり判定
	for (auto& group : WaxManager::GetInstance()->waxGroups)
	{
		std::vector<Enemy*> trapEnemys;
		//蝋一つ一つとの判定
		for (auto& wax : group->waxs) {
			for (auto& enemy : EnemyManager::GetInstance()->enemys)
			{
				bool isCollision = ColPrimitive3D::CheckSphereToSphere(enemy->collider, wax->collider);

				//グループとの判定
				//今のフレームにロウが固まったならロウを足止めする
				if (isCollision && group->GetNowIsSolid())
				{
					trapEnemys.push_back(enemy.get());
				}

				if (isCollision && wax->isSolid == false) {
					//投げられてる蝋に当たった時はダメージと蝋蓄積
					if (wax->isGround == false) {
						//enemyにダメージ
						Vector3 knockVec = player.atkVec;
						knockVec.y = 0;
						enemy->DealDamage(wax->atkPower,
							knockVec, &player.obj);

						//お試し実装:自分が攻撃を当てた相手が自分を追いかけてくる
						if (player.GetTauntMode()) {
							enemy->SetTarget(&player.obj);
						}
					}
					//地面の蝋とぶつかってたら足盗られに
					else
					{
						enemy->ChangeState<EnemySlow>();
					}
				}

				//燃えてるロウと当たったら燃えてる状態に遷移
				if (isCollision && wax->GetState() == "WaxBurning") {
					enemy->ChangeState<EnemyBurning>();
				}

				//回収中ものと通常の状態なら
				if (isCollision && wax->stateStr == "WaxCollect")
				{
					//死ぬ
					enemy->SetDeath();

					player.waxCollectAmount++;
				}
			}
		}

		//敵を一体でも巻き込んでいたらロウが壊れるように
		if (group->GetNowIsSolid() && trapEnemys.size() > 0)
		{
			//1~9までの場合を入れる
			float time = 0.0f;
			
			for (int i = 0; i < 10; i++)
			{
				if (trapEnemys.size() - 1 == i) {
					time = WaxManager::GetInstance()->waxTime[i];
					break;
				}
			}
			//10以上の場合を入れる
			if (trapEnemys.size() - 1 >= 10) {
				time = WaxManager::GetInstance()->waxTime[9];
			}

			//ロウが壊れる時間とエネミーが死ぬ時間を合わせる
			group->breakTimer = time;
			group->breakTimer.Start();
			for (auto& tEnemy : trapEnemys)
			{
				tEnemy->solidTimer = time;
				tEnemy->ChangeState<EnemyAllStop>();
			}
			trapEnemys.clear();
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
	Level::Get()->Update();

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

						//回収中ものと通常の状態なら
						if (wax1->stateStr == "WaxCollect" && wax2->IsNormal())
						{
							//死ぬ
							wax2->DeadParticle();
							wax2->isAlive = false;

							player.waxCollectAmount++;
						}
					}
				}
			}
		}
	}
	

	//別グループ内のロウが当たった時の処理
	for (auto itr = wGroups->begin(); itr != wGroups->end();itr++)
	{
		for (auto itr2 = itr; itr2 != wGroups->end();)
		{
			if (itr == itr2) {
				itr2++;
				continue;
			}
			if (WaxManager::GetInstance()->CheckHitWaxGroups((*itr), (*itr2)))
			{
				//どれか一つがぶつかったなら、グループすべてが移動する
				(*itr)->waxs.splice((*itr)->waxs.end(), std::move((*itr2)->waxs));
				(*itr)->SetSameSolidTime();
				itr2 = wGroups->erase(itr2);
			}
			else {
				itr2++;
			}
		}
	}

#pragma endregion
	WaxManager::GetInstance()->Update();
	FireManager::GetInstance()->Update();
	TemperatureManager::GetInstance()->Update();
	ParticleManager::GetInstance()->SetPlayerPos(player.GetCenterPos());
	ParticleManager::GetInstance()->Update();

	//eggUI.Update();
	//nest.Update();

	Minimap::GetInstance()->Update();

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

	ImGui::Text("スティック x: % f y : % f", stick.x, stick.y);
	ImGui::Text("座標:%f,%f,%f",
		camera.mViewProjection.mEye.x, 
		camera.mViewProjection.mEye.y, 
		camera.mViewProjection.mEye.z);
	ImGui::SliderFloat("カメラ距離:%f", &cameraDist, -500.f, 0.f);
	ImGui::SliderAngle("カメラアングルX:%f", &cameraAngle.x);
	ImGui::SliderAngle("カメラアングルY:%f", &cameraAngle.y);
	ImGui::SliderFloat("カメラの移動速度", &cameraSpeed,0.0f,0.5f);
	ImGui::SliderFloat("ミニマップ用カメラ距離:%f", &mmCameraDist, -1000.f, 0.f);
	
	static bool changeCamera = false;
	static float saveDist = cameraDist;
	static Vector2 saveAngle = cameraAngle;
	
	if (ImGui::Checkbox("上から視点に切り替え", &changeCamera)) {
		if (changeCamera) {
			saveDist = cameraDist;
			saveAngle = cameraAngle;
		}
		else {
			cameraDist = saveDist;
			cameraAngle = saveAngle;
		}
	}

	if (changeCamera) {
		cameraDist = -160.f;
		cameraAngle.x = 70.f;
		cameraAngle.y = 0.f;
	}

	static Vector3 fpsPos;
	static float fpsDist = cameraDist;
	//static Vector2 fpsAngle = cameraAngle;

	if (ImGui::Checkbox("FPS視点に切り替え", &changeFPS)) {
		if (changeFPS) {
			fpsDist = cameraDist;
			//fpsAngle = cameraAngle;
			fpsPos = camera.mViewProjection.mEye;
		}
		else {
			cameraDist = fpsDist;
			//cameraAngle = fpsAngle;
			camera.mViewProjection.mEye = fpsPos;
		}
	}
	if (changeFPS) {
		camera.mViewProjection.mEye = player.GetPos();
		camera.mViewProjection.UpdateMatrix();
		cameraDist = 0.01f;

		if (stick.LengthSq() > 0.0f) {
			float moveSpeed = cameraSpeed;

			if (!std::signbit(stick.y)) {
				moveSpeed *= -1;
			}
			
			cameraAngle.x -= moveSpeed;
		}
	}

	if (ImGui::Button("セーブ")) {
		Parameter::Begin("Camera");
		Parameter::Save("カメラ距離", cameraDist);
		Parameter::Save("カメラアングルX", cameraAngle.x);
		Parameter::Save("カメラアングルY", cameraAngle.y);
		Parameter::Save("カメラの移動速度", cameraSpeed);
		Parameter::Save("ミニマップ用カメラ距離", mmCameraDist);
		Parameter::End();
	}

	ImGui::End();

	ImGui::SetNextWindowSize({ 400, 200 });

	window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	// デバッグモード //
	ImGui::Begin("デバッグ", NULL, window_flags);
	ImGui::Text("デバッグモード中はシーン切り替えが発生しません");
	ImGui::Text("デバッグモードはF5で切り替えもできます");
	if (ImGui::Checkbox("デバッグモード切り替え", &Util::debugBool)) {
		ImGui::Text("デバッグモード中です");
	}
	if (ImGui::Button("セーブ")) {
		Parameter::Begin("DebugBool");
		Parameter::Save("debugBool", Util::debugBool);
		Parameter::End();
	}

	ImGui::End();

	SpawnDataLoader::OrderCreateGUI();

#pragma endregion
}

void ProtoScene::Draw()
{
	Minimap::GetInstance()->Draw();
	ParticleManager::GetInstance()->Draw();
	skydome.Draw();
	WaxManager::GetInstance()->Draw();
	FireManager::GetInstance()->Draw();
	TemperatureManager::GetInstance()->Draw();
	//eggUI.Draw();
	//nest.Draw();

	Level::Get()->Draw();
	player.Draw();

	//更新
	InstantDrawer::AllUpdate();
	InstantDrawer::AllDraw2D();
}

void ProtoScene::MinimapCameraUpdate()
{
	Vector3 mmCameraVec = { 0, 0, 1 };
	//カメラアングル適応
	mmCameraVec *= Quaternion::AngleAxis(Vector3(1, 0, 0).Cross(mmCameraVec), 0.f);
	mmCameraVec *= Quaternion::AngleAxis(Vector3(0, 1, 0).Cross(mmCameraVec), Util::AngleToRadian(89.9f));
	//カメラの距離適応
	mmCameraVec *= mmCameraDist;

	//プレイヤーと一定の距離を保って着いていく
	minimapCamera.mViewProjection.mEye = mmCameraVec;
	//プレイヤーの方向いてくれる
	minimapCamera.mViewProjection.mTarget = Vector3::ZERO;
	minimapCamera.mViewProjection.UpdateMatrix();
}
