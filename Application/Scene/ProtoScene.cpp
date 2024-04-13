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
//#include "Temperature.h"
//#include "FireManager.h"
#include "Parameter.h"
#include "SpawnOrderData.h"
#include "Minimap.h"

ProtoScene::ProtoScene()
{
	TextureManager::Load("./Resources/Brush.png", "brush");

	skydome = ModelObj(Model::Load("./Resources/Model/Skydome/Skydome.obj", "Skydome"));
	skydome.mTransform.scale = { 5, 5, 5 };
	skydome.mTransform.UpdateMatrix();

	//TemperatureUI::LoadResource();
	EnemyManager::LoadResource();
	InstantDrawer::PreCreate();

	Level::Get()->Load();

	wave.Load();

	//EggUI::LoadResource();
}

void ProtoScene::Init()
{
	Camera::sMinimapCamera = &minimapCamera;
	
	gameCamera.Init();

	LightGroup::sNowLight = &light;

	player.Init();
	boss.Init();
	
	//色々入れる
	player.SetBoss(&boss);
	boss.SetTarget(&player.obj);
	gameCamera.SetTarget(&player.obj);


	ParticleManager::GetInstance()->Init();

	WaxManager::GetInstance()->Init();
	//FireManager::GetInstance()->Init();
	//TemperatureManager::GetInstance()->Init();

	//とりあえず最初のステージを設定しておく
	Level::Get()->Extract("test");

	EnemyManager::GetInstance()->SetTarget(&player.obj);

	Minimap::GetInstance()->Init();

	std::map<std::string, std::string> extract = Parameter::Extract("DebugBool");
	Util::debugBool = Parameter::GetParam(extract, "debugBool", false);
}

void ProtoScene::Update()
{
	//初期化周り
	InstantDrawer::DrawInit();
	WaxManager::GetInstance()->slimeWax.Reset();

	gameCamera.Update();

	MinimapCameraUpdate();

	//ここに無限に当たり判定増やしていくの嫌なのであとで何か作ります
	//クソ手抜き当たり判定
	for (size_t i = 0; i < boss.parts.size(); i++)
	{
		if (ColPrimitive3D::CheckSphereToSphere(boss.parts[i].collider,
			player.collider))
		{
			//パンチタイマー進行中のみダメージ
			if (boss.punchTimer.GetRun())
			{
				//1ダメージ(どっかに参照先作るべき)
				player.DealDamage(1);
			}
		}
	}

	for (auto& enemy : EnemyManager::GetInstance()->enemys)
	{
		//タワーとの当たり判定
		if (ColPrimitive3D::CheckSphereToSphere(enemy->collider,
			Level::Get()->tower.collider)) {
			enemy->SetDeath();
			Vector3 vec = Level::Get()->tower.GetPos() - enemy->GetPos();
			Level::Get()->tower.Damage(1.f, vec);
		}
		//プレイヤーとの当たり判定
		//特定範囲内に入ったら敵を攻撃状態へ遷移
		if (enemy->GetAttackState() == "NonAttack")
		{
			if (ColPrimitive3D::CheckSphereToSphere(enemy->attackHitCollider, player.attackHitCollider))
			{
				enemy->ChangeAttackState<EnemyFindState>();
			}
		}
		//攻撃中に本体同士がぶつかったらプレイヤーにダメージ
		if (enemy->GetAttackState() == "NowAttack")
		{
			if (ColPrimitive3D::CheckSphereToSphere(enemy->collider, player.collider))
			{
				//1ダメージ(どっかに参照先作るべき)
				player.DealDamage(1);
			}
		}
	}

	//蝋とボスの当たり判定
	for (auto& group : WaxManager::GetInstance()->waxGroups)
	{
		for (auto& wax : group->waxs)
		{
			//ボス本体との判定
			bool isCollision = ColPrimitive3D::CheckSphereToSphere(boss.collider, wax->collider);

			//投げられてる蝋に当たった時はダメージと蝋蓄積
			if (isCollision &&
				wax->isSolid == false &&
				wax->isGround == false)
			{
				//一応1ダメージ
				boss.DealDamage(1);
			}

			for (size_t i = 0; i < boss.parts.size(); i++)
			{
				//腕との判定
				isCollision = ColPrimitive3D::CheckSphereToSphere(boss.parts[i].collider,
					wax->collider);
				if (isCollision &&
					wax->isSolid == false &&
					wax->isGround == false)
				{
					//一応1ダメージ
					boss.parts[i].DealDamage(1);
				}
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

	//蝋と敵の当たり判定
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

				////燃えてるロウと当たったら燃えてる状態に遷移
				//if (isCollision && wax->GetState() == "WaxBurning") {
				//	enemy->ChangeState<EnemyBurning>();
				//}

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

	//蝋とプレイヤーの当たり判定
	player.isCollect = true;
	for (auto& group : WaxManager::GetInstance()->waxGroups)
	{
		//蝋一つ一つとの判定
		for (auto& wax : group->waxs)
		{
			if (wax->GetState() == "WaxCollectFan")
			{
				bool isCollision = ColPrimitive3D::CheckSphereToSphere(player.collider, wax->collider);
				player.isCollect = false;	//一個でも回収中のロウあると回収できなくする

				if (isCollision)
				{
					wax->isAlive = false;
					player.waxCollectAmount++;
				}
			}
		}
	}

	/*for (auto& fire : FireManager::GetInstance()->fires)
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
	}*/

	player.Update();
	boss.Update();
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
						////燃えているものと通常の状態なら
						//if (wax1->IsBurning() && wax2->IsNormal())
						//{
						//	//燃えている状態へ遷移
						//	wax2->ChangeState<WaxIgnite>();
						//}
					}
				}
			}
		}
	}

	//別グループ内のロウが当たった時の処理
	for (auto itr = wGroups->begin(); itr != wGroups->end(); itr++)
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
	ParticleManager::GetInstance()->SetPlayerPos(player.GetCenterPos());
	ParticleManager::GetInstance()->Update();

	WaxManager::GetInstance()->Update();

	Minimap::GetInstance()->Update();

	light.Update();

	skydome.TransferBuffer(gameCamera.camera.mViewProjection);

	//F6かメニューボタン押されたらリザルトシーンへ
	if (RInput::GetInstance()->GetKeyDown(DIK_F6) ||
		RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_START))
	{
		SceneManager::GetInstance()->Change<ResultScene>();
	}

#pragma region ImGui
	ImGui::SetNextWindowSize({ 400, 200 }, ImGuiCond_FirstUseEver);

	// デバッグモード //
	ImGui::Begin("デバッグ");
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
	//FireManager::GetInstance()->Draw();
	//TemperatureManager::GetInstance()->Draw();
	//eggUI.Draw();
	//nest.Draw();

	Level::Get()->Draw();
	boss.Draw();
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
	mmCameraVec *= gameCamera.mmCameraDist;

	//アスペクト比1:1に
	minimapCamera.mViewProjection.mAspect = 1.f / 1.f;

	minimapCamera.mViewProjection.mEye = mmCameraVec;

	minimapCamera.mViewProjection.mTarget = Vector3::ZERO;
	minimapCamera.mViewProjection.UpdateMatrix();
}
