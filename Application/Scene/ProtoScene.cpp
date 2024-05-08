#include "ProtoScene.h"
#include "FailedScene.h"
#include "SceneManager.h"
#include "WaxManager.h"
#include "ParticleManager.h"
#include "RInput.h"
#include "RImGui.h"
#include <Quaternion.h>
#include "ColPrimitive3D.h"
#include "InstantDrawer.h"
#include "Parameter.h"
#include "SpawnOrderData.h"
#include "Minimap.h"
//#include "CollectPartManager.h"
#include "BossAppearanceScene.h"
#include "BossDeadScene.h"
#include "SceneTrance.h"
#include "EventCaller.h"
#include "TitleScene.h"
#include "SimpleSceneTransition.h"
#include "RAudio.h"
#include "Boss.h"

ProtoScene::ProtoScene()
{
	TextureManager::Load("./Resources/Brush.png", "brush");
	//CollectPartManager::LoadResouces();

	skydome = ModelObj(Model::Load("./Resources/Model/bg/bg.obj", "bg"));
	skydome.mTransform.scale = { 1.5f, 1.5f, 1.5f };
	skydome.mTransform.UpdateMatrix();

	EnemyManager::LoadResource();
	InstantDrawer::PreCreate();

	Level::Get()->Load();

	RAudio::Load("Resources/Sounds/BGM/Ingame.wav", "Normal");
	RAudio::Load("Resources/Sounds/BGM/Boss.wav", "Boss");
	RAudio::Load("Resources/Sounds/SE/P_attackHit.wav", "Hit");

	ControlUI::LoadResource();
}

void ProtoScene::Init()
{
	EventCaller::Init();
	//Camera::sMinimapCamera = &minimapCamera;

	gameCamera.Init();

	LightGroup::sNowLight = &light;

	player.Init();
	Boss::GetInstance()->Init();

	//色々入れる
	player.SetBoss(Boss::GetInstance());
	Boss::GetInstance()->SetTarget(&player.obj);
	gameCamera.SetTarget(&player.obj);

	ParticleManager::GetInstance()->Init();

	WaxManager::GetInstance()->Init();

	//とりあえず最初のステージを設定しておく
	Level::Get()->Extract("test");

	EnemyManager::GetInstance()->SetTarget(&player.obj);

	//Minimap::GetInstance()->Init();

	RAudio::Stop("Normal");
	RAudio::Stop("Boss");

	RAudio::Play("Normal", 0.5f, 1.0f, true);

	std::map<std::string, std::string> extract = Parameter::Extract("DebugBool");
	Util::debugBool = Parameter::GetParam(extract, "debugBool", false);

	/*CollectPartManager::GetInstance()->Init();
	CollectPartManager::GetInstance()->zone.pos = { 100,0,100 };
	CollectPartManager::GetInstance()->zone.scale = { 100,100 };

	CollectPartManager::GetInstance()->SetPlayer(&player);*/

	controlUI.Init();

	manager.Init(&light);
}

void ProtoScene::Update()
{
	//初期化周り
	InstantDrawer::DrawInit();
	WaxManager::GetInstance()->slimeWax.Reset();

	//ボス撃破シーンに切り替え
	if (Util::debugBool) {
		if (RInput::GetInstance()->GetKeyDown(DIK_B))
		{
			RAudio::Stop("Boss");
		    RAudio::Stop("Normal");
			EventCaller::EventCall(BossDeadScene::GetEventCallStr());
			player.isMove = false;
		}

		//ボス登場シーンに切り替え
		if (RInput::GetInstance()->GetKeyDown(DIK_T))
		{
			RAudio::Stop("Boss");
			RAudio::Stop("Normal");
			EventCaller::EventCall(BossAppearanceScene::GetEventCallStr());
			player.isMove = false;
		}
	}

	SceneTrance::GetInstance()->Update();

	//ボス撃破シーンに遷移
	if ((EventCaller::GetEventCallStr() == BossDeadScene::GetEventCallStr()) &&
		SceneTrance::GetInstance()->GetIsChange())
	{
		EventCaller::eventScene = std::make_unique<BossDeadScene>();
		EventCaller::eventScene->Init(Boss::GetInstance()->GetCenterPos() + Vector3::UP * 20.f);

		RAudio::Stop("Boss");
		RAudio::Stop("Normal");

		player.isMove = false;
		Boss::GetInstance()->isDead = true;

		SceneTrance::GetInstance()->SetIsChange(false);	//忘れずに
		EventCaller::EventCallStrReset();
	}

	//ボス登場シーンに遷移
	if ((EventCaller::GetEventCallStr() == BossAppearanceScene::GetEventCallStr()) &&
		SceneTrance::GetInstance()->GetIsChange())
	{
		EventCaller::eventScene = std::make_unique<BossAppearanceScene>();
		EventCaller::eventScene->Init(Boss::GetInstance()->GetCenterPos() + Vector3::UP * 20.f);

		RAudio::Stop("Boss");
		RAudio::Stop("Normal");
		RAudio::Play("Boss", 0.5f, 1.0f, true);

		player.isMove = false;
		Boss::GetInstance()->isAppearance = true;
		Boss::GetInstance()->isAlive = true;
		EnemyManager::GetInstance()->isStop = true;

		SceneTrance::GetInstance()->SetIsChange(false);	//忘れずに
		EventCaller::EventCallStrReset();
	}

	//イベントシーン中なら
	EventCaller::Update();

	//イベントシーンが終わりカメラが空っぽになったら
	if (Camera::sNowCamera == nullptr)
	{
		gameCamera.Init();	//カメラ入れる
		player.isMove = true;
		EnemyManager::GetInstance()->isStop = false;

		//今後まとめるときは、End()みたいな項目でこれらを呼べるようにしたい
		//登場演出なら、ボスの登場演出モードを解除
		if (EventCaller::GetNowEventStr() == BossAppearanceScene::GetEventCallStr()) {
			Boss::GetInstance()->isAppearance = false;
		}

		//死亡シーンの呼び出しが終わったならタイトルに戻す
		if (EventCaller::GetNowEventStr() == BossDeadScene::GetEventCallStr()) {
			RAudio::Stop("Boss");
			RAudio::Stop("Normal");
			SceneManager::GetInstance()->Change<TitleScene,SimpleSceneTransition>();
		}

		EventCaller::NowEventStrReset();
	}

	gameCamera.Update();
	//MinimapCameraUpdate();

	//ここに無限に当たり判定増やしていくの嫌なのであとで何か作ります
	//クソ手抜き当たり判定

	//ボスの腕との判定
	for (size_t i = 0; i < Boss::GetInstance()->parts.size(); i++)
	{
		if (ColPrimitive3D::CheckSphereToSphere(Boss::GetInstance()->parts[i].collider,
			player.collider))
		{
			//パンチタイマー進行中のみダメージ
			if (Boss::GetInstance()->punchTimer.GetRun())
			{
				//1ダメージ(どっかに参照先作るべき)
				player.DealDamage(1);
			}
		}
	}

	////パーツとの判定
	//for (auto& part : CollectPartManager::GetInstance()->parts)
	//{
	//	if ((int32_t)player.carryingParts.size() <
	//		CollectPartManager::GetInstance()->GetMaxCarryingNum()) {
	//		if (ColPrimitive3D::CheckSphereToSphere(part->collider, player.collider)) {
	//			//一旦複数持てる
	//			//後でプレイヤー側でフラグ立てて個数制限する
	//			part->Carrying(&player);
	//		}
	//	}

	//	//プレイヤーが持っているなら
	//	if (part->IsCarrying()) {
	//		//当たり判定する
	//		if (ColPrimitive3D::CheckSphereToAABB(player.collider,
	//			CollectPartManager::GetInstance()->zone.aabbCol)) {
	//			part->Collect();
	//			part->SetIsAlive(false);
	//			CollectPartManager::GetInstance()->zone.Create(*part);
	//		}
	//	}
	//}

	for (auto itr = player.carryingParts.begin(); itr != player.carryingParts.end();)
	{
		//捕まったら保持から消す
		if ((*itr)->IsCollected()) {
			itr = player.carryingParts.erase(itr);
		}
		else {
			itr++;
		}
	}

	bool isHitSound = false;

	//蝋とボスの当たり判定
	for (auto& group : WaxManager::GetInstance()->waxGroups)
	{
		for (auto& wax : group->waxs)
		{
			//ボス本体との判定
			bool isCollision = ColPrimitive3D::CheckSphereToSphere(Boss::GetInstance()->collider, wax->collider);

			//投げられてる蝋に当たった時はダメージと蝋蓄積
			if (isCollision && wax->isSolid == false && wax->isGround == false)
			{
				//一応1ダメージ(ダメージ量に応じてロウのかかり具合も進行)
				Boss::GetInstance()->DealDamage(player.GetAttackPower());
				isHitSound = true;
			}

			for (size_t i = 0; i < Boss::GetInstance()->parts.size(); i++)
			{
				//腕との判定
				isCollision = ColPrimitive3D::CheckSphereToSphere(Boss::GetInstance()->parts[i].collider, wax->collider);
				if (isCollision && wax->isSolid == false && wax->isGround == false)
				{
					//一応1ダメージ(ダメージ量に応じてロウのかかり具合も進行)
					Boss::GetInstance()->parts[i].DealDamage(player.GetAttackPower());
					isHitSound = true;
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
		//蝋一つ一つとの判定
		for (auto& wax : group->waxs) {
			for (auto& enemy : EnemyManager::GetInstance()->enemys)
			{
				bool isCollision = ColPrimitive3D::CheckSphereToSphere(enemy->collider, wax->collider);

				if (isCollision && wax->isSolid == false) {
					//投げられてる蝋に当たった時はダメージと蝋蓄積
					if (wax->isGround == false) {
						//enemyにダメージ
						Vector3 knockVec = player.atkVec;
						knockVec.y = 0;
						enemy->DealDamage(player.GetAttackPower(),
							knockVec, &player.obj);
						isHitSound = true;
					}
					//地面の蝋とぶつかってたら足盗られに
					else
					{
						enemy->ChangeState<EnemySlow>();
					}
				}

				//回収中ものと通常の状態なら
				if (isCollision && wax->stateStr == "WaxCollect")
				{
					//固まってないならダメージ
					if (!enemy->GetIsSolid())
					{
						Vector3 knockVec = -player.atkVec;
						knockVec.y = 0;
						enemy->DealDamage(player.GetInvolvePower(),
							knockVec, &player.obj);

						//このタイミングで固まった場合も溶ける
						if (enemy->GetIsSolid())
						{
							//回収状態に遷移
							enemy->collectPos = player.GetPos();
							enemy->isCollect = true;
							enemy->ChangeState<EnemyCollect>();

							player.waxCollectAmount++;
						}
					}
				}
			}
		}
	}

	//吸収ボタンを押して、吸収状態の敵が一匹もいないなら吸収できる
	bool isCollected2 = player.GetWaxCollectButtonDown() && !EnemyManager::GetInstance()->GetNowCollectEnemy();

	for (auto& enemy : EnemyManager::GetInstance()->enemys)
	{
		//プレイヤーとの当たり判定
		//特定範囲内に入ったら敵を攻撃状態へ遷移
		if (enemy->GetAttackState() == EnemyNormalState::GetStateStr())
		{
			if (ColPrimitive3D::CheckSphereToSphere(enemy->attackHitCollider, player.attackHitCollider))
			{
				enemy->ChangeAttackState<EnemyFindState>();
			}
		}
		//攻撃中に本体同士がぶつかったらプレイヤーにダメージ
		if (enemy->GetAttackState() == EnemyNowAttackState::GetStateStr())
		{
			if (ColPrimitive3D::CheckSphereToSphere(enemy->collider, player.collider))
			{
				//ダメージ
				player.DealDamage(EnemyManager::GetInstance()->GetNormalAttackPower());
			}
		}
		//接触ダメージあり設定の場合、接触時にダメージ
		if (EnemyManager::GetInstance()->GetIsContactDamage()) {
			if (ColPrimitive3D::CheckSphereToSphere(enemy->collider, player.collider))
			{
				//ダメージ
				player.DealDamage(EnemyManager::GetInstance()->GetContactAttackPower());
			}
		}
		//回収ボタン押されたときに固まってるなら吸収
		//今は範囲外でも吸収できちゃってる
		//ここもプレイヤーの中に入れちゃう
		if (isCollected2 && enemy->GetIsSolid() &&
			ColPrimitive3D::RayToSphereCol(player.collectCol, enemy->collider))
		{
			//回収状態に遷移
			enemy->collectPos = player.GetPos();
			enemy->isCollect = true;
			enemy->ChangeState<EnemyCollect>();

			player.waxCollectAmount++;
		}
	}

	if (isHitSound && !player.soundFlag) {
		//ここで攻撃のヒット音を鳴らす
		RAudio::Play("Hit");
		//フラグを立てる
		player.soundFlag = true;
	}

	//弾とプレイヤーの判定
	for (auto& shot : EnemyManager::GetInstance()->enemyShots)
	{
		if (ColPrimitive3D::CheckSphereToSphere(shot->collider, player.collider)) {
			shot->SetIsAlive(false);
			player.DealDamage(shot->GetDamage());
		}
	}


	player.Update();
	Boss::GetInstance()->Update();
	Level::Get()->Update();

	//敵同士の押し戻し
	for (auto& enemy1 : EnemyManager::GetInstance()->enemys)
	{
		for (auto& enemy2 : EnemyManager::GetInstance()->enemys)
		{
			if (enemy1 == enemy2)continue;

			if (ColPrimitive3D::CheckSphereToSphere(enemy1->collider, enemy2->collider)) {
				Vector3 e1RepulsionVec = enemy1->GetPos() - enemy2->GetPos();
				e1RepulsionVec.Normalize();
				e1RepulsionVec.y = 0;
				Vector3 e2RepulsionVec = enemy2->GetPos() - enemy1->GetPos();
				e2RepulsionVec.Normalize();
				e2RepulsionVec.y = 0;

				//一旦これだけ無理やり足す
				enemy1->obj.mTransform.position += e1RepulsionVec;
				enemy2->obj.mTransform.position += e2RepulsionVec;
				
				//プレイヤーを探している状態(攻撃状態でない時)に他の敵にぶつかった場合
				//周回座標の基準をずらす
				if (enemy1->GetAttackState() == EnemyNormalState::GetStateStr())
				{
					enemy1->BehaviorOrigenPosPlus(e1RepulsionVec);
				}
				if (enemy1->GetAttackState() == EnemyNormalState::GetStateStr())
				{
					enemy2->BehaviorOrigenPosPlus(e2RepulsionVec);
				}

				//コライダーがもう一度当たらないようにコライダー更新
				enemy1->UpdateCollider();
				enemy2->UpdateCollider();
			}
		}
	}

	//ロウグループ周りの話は無くなったはずなのでコメントアウト
//	//敵がロウを壊してから連鎖で壊れるため、敵の処理をしてからこの処理を行う
//#pragma region ロウ同士の当たり判定
//	std::list<std::unique_ptr<WaxGroup>>* wGroups = &WaxManager::GetInstance()->waxGroups;
//
//	//同一グループ内のロウが当たった時の処理
//	for (auto& group1 : *wGroups)
//	{
//		for (auto& group2 : *wGroups)
//		{
//			for (auto& wax1 : group1->waxs) {
//				for (auto& wax2 : group2->waxs)
//				{
//					//同じ部分を指しているポインタなら同じものなのでスキップ
//					if (wax1 == wax2)continue;
//
//					bool isCollision = ColPrimitive3D::CheckSphereToSphere(wax1->collider, wax2->collider);
//
//					//ぶつかっていて
//					if (isCollision)
//					{
//						////燃えているものと通常の状態なら
//						//if (wax1->IsBurning() && wax2->IsNormal())
//						//{
//						//	//燃えている状態へ遷移
//						//	wax2->ChangeState<WaxIgnite>();
//						//}
//					}
//				}
//			}
//		}
//	}
//
//	//別グループ内のロウが当たった時の処理
//	for (auto itr = wGroups->begin(); itr != wGroups->end(); itr++)
//	{
//		for (auto itr2 = itr; itr2 != wGroups->end();)
//		{
//			if (itr == itr2) {
//				itr2++;
//				continue;
//			}
//			if (WaxManager::GetInstance()->CheckHitWaxGroups((*itr), (*itr2)))
//			{
//				//どれか一つがぶつかったなら、グループすべてが移動する
//				(*itr)->waxs.splice((*itr)->waxs.end(), std::move((*itr2)->waxs));
//				(*itr)->SetSameSolidTime();
//				itr2 = wGroups->erase(itr2);
//			}
//			else {
//				itr2++;
//			}
//		}
//	}
//
//#pragma endregion

	ParticleManager::GetInstance()->SetPlayerPos(player.GetCenterPos());
	ParticleManager::GetInstance()->Update();

	WaxManager::GetInstance()->Update();
	//CollectPartManager::GetInstance()->Update();

	//Minimap::GetInstance()->Update();

	manager.Imgui();
	manager.Update();

	light.Update();

	skydome.TransferBuffer(Camera::sNowCamera->mViewProjection);

	//F6かメニューボタン押されたらリザルトシーンへ
	if (RInput::GetInstance()->GetKeyDown(DIK_F6) ||
		RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_START))
	{
		RAudio::Stop("Boss");
		RAudio::Stop("Normal");
		SceneManager::GetInstance()->Change<FailedScene,SimpleSceneTransition>();
	}

	if (RInput::GetInstance()->GetKeyDown(DIK_J))
	{
		ParticleManager::GetInstance()->AddHoming(player.GetPos(), "enemy_solid_homing");
	}
	controlUI.Update();

#pragma region ImGui
	if (RImGui::showImGui)
	{

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
	}

	SpawnDataLoader::OrderCreateGUI();

#pragma endregion
}

void ProtoScene::Draw()
{
	manager.Draw();

	//Minimap::GetInstance()->Draw();
	ParticleManager::GetInstance()->Draw();
	skydome.Draw();
	WaxManager::GetInstance()->Draw();
	//CollectPartManager::GetInstance()->Draw();

	Level::Get()->Draw();
	Boss::GetInstance()->Draw();
	player.Draw();

	EventCaller::Draw();

	controlUI.Draw();

	//更新
	InstantDrawer::AllUpdate();
	InstantDrawer::AllDraw2D();

	SceneTrance::GetInstance()->Draw();
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
