#include "ProtoScene.h"
#include "FailedScene.h"
#include "SceneManager.h"
#include "GameCamera.h"
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
#include "WaxSceneTransition.h"
#include "RAudio.h"
#include "Boss.h"
#include "LightObject.h"
#include "NumDrawer.h"
#include "Wave.h"
#include "Tank.h"
#include "BombSolider.h"
#include "MoveTutorialScene.h"
#include "AttackTutorialScene.h"
#include "ParryTutorialScene.h"

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
	RAudio::Load("Resources/Sounds/BGM/Gameclear.wav", "Clear");

	RAudio::Load("Resources/Sounds/SE/P_attackHit.wav", "Hit");
	RAudio::Load("Resources/Sounds/SE/playerParry.wav", "Parry");
	RAudio::Load("Resources/Sounds/SE/playerGuard.wav", "Guard");

	ControlUI::LoadResource();
	TimerUI::LoadResource();
	NumDrawer::LoadResource();

	WaveManager::Get()->LoadLevelData();

	waveUI.LoadResource();
}

void ProtoScene::Init()
{
	EventCaller::Init();
	//Camera::sMinimapCamera = &minimapCamera;

	GameCamera::GetInstance()->Init();

	LightGroup::sNowLight = &light;
	ambient = light.GetAmbientColor();

	player.Init();
	Boss::GetInstance()->Init();

	//色々入れる
	player.SetBoss(Boss::GetInstance());
	Boss::GetInstance()->SetTarget(&player.obj);
	GameCamera::GetInstance()->SetTarget(&player.obj);

	ParticleManager::GetInstance()->Init();

	WaxManager::GetInstance()->Init();

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

	extract = Parameter::Extract("Boss");
	controlUI.Init();
	bossAppTimerUI.Init();

	SpotLightManager::GetInstance()->Init(&light);

	WaveManager::Get()->Init();

	waveUI.Init();
}

void ProtoScene::Update()
{
	//初期化周り
	InstantDrawer::DrawInit();
	WaxManager::GetInstance()->slimeWax.Reset();
	player.Reset();

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

	if (player.hp <= 0)
	{
		if (RAudio::IsPlaying("Boss") || RAudio::IsPlaying("Normal"))
		{
			RAudio::Stop("Boss");
			RAudio::Stop("Normal");
		}
	}

	SceneTrance::GetInstance()->Update();

	if ((EventCaller::GetEventCallStr() == MoveTutorialScene::GetEventCallStr())) {
		//移動のチュートリアルに遷移
		EventCaller::GetInstance()->eventScene = std::make_unique<MoveTutorialScene>();
		EventCaller::GetInstance()->eventScene->Init(Camera::sNowCamera->mViewProjection.mTarget);

		EventCaller::EventCallStrReset();
	}
	else if ((EventCaller::GetEventCallStr() == AttackTutorialScene::GetEventCallStr())) {
		//攻撃のチュートリアルに遷移
		EventCaller::GetInstance()->eventScene = std::make_unique<AttackTutorialScene>();
		EventCaller::GetInstance()->eventScene->Init(Camera::sNowCamera->mViewProjection.mTarget);

		EventCaller::EventCallStrReset();
	}
	else if ((EventCaller::GetEventCallStr() == ParryTutorialScene::GetEventCallStr())) {
		//パリィのチュートリアルに遷移
		EventCaller::GetInstance()->eventScene = std::make_unique<ParryTutorialScene>();
		EventCaller::GetInstance()->eventScene->Init(Camera::sNowCamera->mViewProjection.mTarget);

		player.isMove = false;
		EnemyManager::GetInstance()->isStop = true;

		EventCaller::EventCallStrReset();
	}

	//移動チュートリアルが終わっておらず、イベントシーンではないなら
	if (player.moveTutorial == false && EventCaller::GetEventCallStr() == "")
	{
		player.moveTutorial = true;
		EventCaller::EventCall(MoveTutorialScene::GetEventCallStr(), false);
	}

	////移動チュートリアルが終わってて、イベントシーンではなく、攻撃チュートリアルが終わってないなら
	//if (player.moveTutorial && player.attackTutorial == false &&
	//	EventCaller::GetEventCallStr() == "")
	//{
	//	EventCaller::EventCall(AttackTutorialScene::GetEventCallStr(), false);
	//	player.attackTutorial = true;
	//}

	//ボス撃破シーンに遷移
	if ((EventCaller::GetEventCallStr() == BossDeadScene::GetEventCallStr()) &&
		SceneTrance::GetInstance()->GetIsChange())
	{
		EventCaller::GetInstance()->eventScene = std::make_unique<BossDeadScene>();
		EventCaller::GetInstance()->eventScene->Init(Boss::GetInstance()->GetCenterPos() + Vector3::UP * 20.f);

		RAudio::Stop("Boss");
		RAudio::Stop("Normal");

		player.isMove = false;
		player.SetIsGodmode(true);	//プレイヤー無敵に
		Boss::GetInstance()->isDead = true;

		SceneTrance::GetInstance()->SetIsChange(false);	//忘れずに
		EventCaller::EventCallStrReset();
	}

	//ボス登場シーンに遷移
	if ((EventCaller::GetEventCallStr() == BossAppearanceScene::GetEventCallStr()) &&
		SceneTrance::GetInstance()->GetIsChange())
	{
		EventCaller::GetInstance()->eventScene = std::make_unique<BossAppearanceScene>();
		EventCaller::GetInstance()->eventScene->Init(Boss::GetInstance()->GetCenterPos() + Vector3::UP * 20.f);

		RAudio::Stop("Boss");
		RAudio::Stop("Normal");
		RAudio::Play("Boss", 0.65f, 1.0f, true);

		player.isMove = false;
		Boss::GetInstance()->isAppearance = true;
		Boss::GetInstance()->isAppearanced = true;
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
		Camera::sNowCamera = &GameCamera::GetInstance()->camera;
		//カメラ入れる
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
			SceneManager::GetInstance()->Change<TitleScene, WaxSceneTransition>();
		}

		Camera::sNowCamera = EventCaller::saveCamera;
		EventCaller::saveCamera = nullptr;

		EventCaller::NowEventStrReset();
	}

	GameCamera::GetInstance()->Update();
	//MinimapCameraUpdate();

	//ここに無限に当たり判定増やしていくの嫌なのであとで何か作ります
	//クソ手抜き当たり判定

	//ボスの腕との判定
	for (size_t i = 0; i < Boss::GetInstance()->parts.size(); i++)
	{
		if (player.GetWaxWall()) {
			if (ColPrimitive3D::CheckSphereToSphere(Boss::GetInstance()->parts[i].collider,
				player.GetWaxWall()->collider) &&
				Boss::GetInstance()->punchImpactTimer.GetRun())
			{
				while (ColPrimitive3D::CheckSphereToSphere(Boss::GetInstance()->parts[i].collider,
					player.GetWaxWall()->collider))
				{
					//パリィが出来てるなら、攻撃を受けてのけぞってる間はパリィタイマーを更新し続ける
					if (player.GetWaxWall()->parryTimer.GetRun()) {
						player.GetWaxWall()->parryTimer.Start();
					}

					Vector3 repulsionVec = player.GetPos() - Boss::GetInstance()->parts[i].collider.pos;
					repulsionVec.Normalize();
					repulsionVec.y = 0;

					//一旦これだけ無理やり足す
					player.obj.mTransform.position += repulsionVec;
					//コライダーがもう一度当たらないようにコライダー更新
					player.UpdateCollider();

					//盾も押し戻す
					player.GetWaxWall()->obj.mTransform.position += repulsionVec;
					player.GetWaxWall()->UpdateCollider();

					//壁にめり込む場合は押し戻し
					for (auto& wall : Level::Get()->wallCol)
					{
						if (ColPrimitive3D::CheckSphereToPlane(player.collider, wall))
						{
							repulsionVec = player.GetPos() - Boss::GetInstance()->parts[i].collider.pos;
							repulsionVec.Normalize();
							repulsionVec.y = 0;
							repulsionVec = -repulsionVec;
							//そのまま戻すと無限ループするので適当に弾き飛ばす
							repulsionVec.x += Util::GetRand(-1.0f, 1.0f);
							repulsionVec.z += Util::GetRand(-1.0f, 1.0f);

							//一旦これだけ無理やり足す
							player.obj.mTransform.position += repulsionVec;
							//コライダーがもう一度当たらないようにコライダー更新
							player.UpdateCollider();

							//盾も押し戻す
							player.GetWaxWall()->obj.mTransform.position += repulsionVec;
							player.GetWaxWall()->UpdateCollider();
						}
					}
				}
				if (!player.GetWaxWall()->parryTimer.GetRun()) {
					player.WaxLeakOut((int32_t)Boss::GetInstance()->GetDamage());
				}
			}
		}

		if (ColPrimitive3D::CheckSphereToSphere(Boss::GetInstance()->parts[i].collider,
			player.collider))
		{
			//パンチタイマー進行中のみダメージ
			if (Boss::GetInstance()->punchImpactTimer.GetRun())
			{
				//1ダメージ(どっかに参照先作るべき)
				player.DealDamage(Boss::GetInstance()->GetDamage());
			}
		}
	}

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

			//体だけになって投げられてる蝋に当たった時はダメージと蝋蓄積
			if (isCollision && wax->isSolid == false && wax->isGround == false &&
				Boss::GetInstance()->GetIsOnlyBody())
			{
				//一応1ダメージ(ダメージ量に応じてロウのかかり具合も進行)
				Boss::GetInstance()->DealDamage(player.GetAttackPower());
				isHitSound = true;
			}

			for (size_t i = 0; i < Boss::GetInstance()->parts.size(); i++)
			{
				//腕との判定
				isCollision = ColPrimitive3D::CheckSphereToSphere(Boss::GetInstance()->parts[i].collider, wax->collider);
				if (isCollision && (wax->stateStr == WaxCollect::GetStateStr() || (wax->isSolid == false && wax->isGround == false)))
				{
					//一応1ダメージ(ダメージ量に応じてロウのかかり具合も進行)
					Boss::GetInstance()->parts[i].DealDamage(player.GetAttackPower());
					isHitSound = true;

					//当たった場所にロウを付着
					if (wax->isSolid == false) {
						Boss::GetInstance()->parts[i].CreateWaxVisual(wax->obj.mTransform.position);
						wax->isSolid = true;
					}
				}
			}
		}
	}

	for (auto& enemy : EnemyManager::GetInstance()->enemys)
	{
		//蝋と当たり判定をする前に、足盗られは毎フレーム解除判定を行う
		if (enemy->GetState() == EnemySlow::GetStateStr()) {
			enemy->ChangeState<EnemyNormal>();
		}
	}

	for (auto& enemy : EnemyManager::GetInstance()->enemys)
	{
		//敵が未出現状態なら飛ばす
		if (!enemy->GetIsSpawn())continue;

		//蝋と敵の当たり判定
		for (auto& group : WaxManager::GetInstance()->waxGroups)
		{
			//蝋一つ一つとの判定
			for (auto& wax : group->waxs) {
				if (enemy->enemyTag == Tank::GetEnemyTag()) {

					Tank* tank = static_cast<Tank*>(enemy.get());

					bool isShieldCollision = tank->GetShield()->GetHitCollider(wax->collider);

					//投げられてるロウと盾がぶつかったらロウを反射
					if (isShieldCollision && wax->isSolid == false && wax->isGround == false) {
						wax->isReverse = true;
						tank->GetShield()->Hit(1);

						if (!RAudio::IsPlaying("Guard"))
						{
							RAudio::Play("Guard", 0.8f);
						}
					}
				}

				bool isCollision = ColPrimitive3D::CheckSphereToSphere(enemy->collider, wax->collider);

				if (isCollision) {
					//投げられてる蝋に当たった時はダメージと蝋蓄積
					if (wax->isGround == false) {
						//enemyにダメージ
						Vector3 knockVec = player.atkVec;
						knockVec.y = 0;
						enemy->DealDamage(player.GetAttackPower(),
							knockVec, &player.obj);
						isHitSound = true;

						//当たった場所にロウを付着
						if (wax->isSolid == false) {
							enemy->CreateWaxVisual(wax->obj.mTransform.position);
							wax->isSolid = true;
						}
					}
					//地面の蝋とぶつかってたら足盗られに
					else
					{
						enemy->ChangeState<EnemySlow>();
					}
				}

				//回収中ものと通常の状態なら
				if (isCollision && wax->stateStr == WaxCollect::GetStateStr())
				{
					//固まってないならダメージ
					if (!enemy->GetIsSolid())
					{
						Vector3 knockVec = -player.atkVec;
						knockVec.y = 0;
						enemy->DealDamage(player.GetInvolvePower(),
							knockVec, &player.obj);

						if (!RAudio::IsPlaying("Hit"))
						{
							RAudio::Play("Hit");
						}

						//このタイミングで固まった場合も溶ける
						if (enemy->GetIsSolid())
						{
							//回収状態に遷移
							enemy->collectPos = player.GetPos();
							enemy->isCollect = true;
							enemy->ChangeState<EnemyCollect>();
						}

						//当たった場所にロウを付着
						if (wax->isSolid == false) {
							enemy->CreateWaxVisual(wax->obj.mTransform.position);
							wax->isSolid = true;
						}
					}
				}
			}
		}
	}

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
			//パリィのチュートリアルを呼ぶ
			if (!player.parryTutorial && WaveManager::Get()->GetNowWave() >= 1) {
				ColPrimitive3D::Sphere eventCollider = player.GetWaxWall(true)->collider;
				eventCollider.r *= 1.5f;
				if (ColPrimitive3D::CheckSphereToSphere(enemy->collider,
					eventCollider) &&
					enemy->GetIsSolid() == false)
				{
					player.parryTutorial = true;
					player.eventParry = true;
					EventCaller::EventCall(ParryTutorialScene::GetEventCallStr(), false);
				}
			}

			if (ColPrimitive3D::CheckSphereToSphere(enemy->collider, player.collider) &&
				enemy->GetIsSolid() == false)
			{
				//チュートリアルなら無理やりパリィ成功したこととする
				if (player.eventParry) {
					if (enemy->enemyTag == Tank::GetEnemyTag())
					{
						enemy->ChangeAttackState<EnemyEndAttackState>();
						Tank* tank = static_cast<Tank*>(enemy.get());

						tank->GetShield()->Break();

						if (!RAudio::IsPlaying("Parry"))
						{
							RAudio::Play("Parry", 0.8f);
						}
						player.eventParry = false;

						while (ColPrimitive3D::CheckSphereToSphere(enemy->collider,
							player.GetWaxWall(true)->collider))
						{
							//プレイヤーと判定、当たってるなら押し戻す
							Vector3 repulsionVec = enemy->GetPos() - player.GetWaxWall(true)->collider.pos;
							repulsionVec.Normalize();
							repulsionVec.y = 0;

							//一旦これだけ無理やり足す
							enemy->obj.mTransform.position += repulsionVec;
							enemy->collider.pos += repulsionVec;

							//コライダーがもう一度当たらないようにコライダー更新
							enemy->UpdateCollider();
						}
					}
				}
				else {
					//ダメージ
					player.DealDamage(EnemyManager::GetInstance()->GetNormalAttackPower());
				}
			}
			//攻撃中に本体と盾がぶつかったらそこで敵をストップ(Endに遷移)
			if (player.GetWaxWall()) {
				if (ColPrimitive3D::CheckSphereToSphere(enemy->collider, player.GetWaxWall()->collider) &&
					enemy->GetIsSolid() == false)
				{
					enemy->ChangeAttackState<EnemyEndAttackState>();
					//もしパリィ中なら盾を吹っ飛ばす
					if (enemy->enemyTag == Tank::GetEnemyTag()) {
						Tank* tank = static_cast<Tank*>(enemy.get());
						if (player.GetWaxWall()->GetParry()) {
							tank->GetShield()->Break();

							if (!RAudio::IsPlaying("Parry"))
							{
								RAudio::Play("Parry", 0.8f);
							}
						}
						else
						{
							int32_t consum = (int32_t)EnemyManager::GetInstance()->GetNormalAttackPower();
							consum = max(consum, 1);

							player.WaxLeakOut(consum);

							if (!RAudio::IsPlaying("Guard"))
							{
								RAudio::Play("Guard", 0.8f);
							}
						}
					}
				}
			}
		}
		//接触ダメージあり設定の場合、固まってないなら接触時にダメージ
		if (EnemyManager::GetInstance()->GetIsContactDamage()) {
			if (ColPrimitive3D::CheckSphereToSphere(enemy->collider, player.collider) && enemy->GetIsSolid() == false)
			{
				//ダメージ
				player.DealDamage(EnemyManager::GetInstance()->GetContactAttackPower());
			}
		}
		////回収ボタン押されたときに固まってるなら吸収
		//if (isCollected2 && enemy->GetIsSolid() &&
		//	ColPrimitive3D::RayToSphereCol(player.collectCol, enemy->collider))
		//{
		//	//回収状態に遷移
		//	enemy->collectPos = player.GetPos();
		//	enemy->isCollect = true;
		//	enemy->ChangeState<EnemyCollect>();
		//}
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
		//パリィのチュートリアルを呼ぶ
		if (!player.parryTutorial && WaveManager::Get()->GetNowWave() >= 1) {
			ColPrimitive3D::Sphere eventCollider = player.GetWaxWall(true)->collider;
			eventCollider.r *= 1.5f;
			if (ColPrimitive3D::CheckSphereToSphere(shot->collider,eventCollider))
			{
				player.parryTutorial = true;
				player.eventParry = true;
				EventCaller::EventCall(ParryTutorialScene::GetEventCallStr(), false);
			}
		}

		//弾が盾と衝突したら
		if (player.GetWaxWall()) {
			if (ColPrimitive3D::CheckSphereToSphere(shot->collider, player.GetWaxWall()->collider))
			{
				if (player.GetWaxWall()->GetParry()) {
					//パリィ出来たら跳ね返す
					shot->Reversal();
					
					if (!RAudio::IsPlaying("Parry"))
					{
						RAudio::Play("Parry", 0.8f);
					}

				}
				else
				{
					//出来ないならロウ消費して防ぐ
					int32_t consum = (int32_t)shot->GetDamage();
					consum = max(consum, 1);

					player.WaxLeakOut(consum);
					shot->SetIsAlive(false);

					RAudio::Play("Guard", 0.8f);
				}

				//判定に成功した時点で、プレイヤーと衝突しないように処理をスキップ
				continue;
			}
		}

		//本体とぶつかったらダメージ
		if (ColPrimitive3D::CheckSphereToSphere(shot->collider, player.collider)) {
			//チュートリアルなら無理やりパリィ成功したこととする
			if (player.eventParry) {
				//パリィ出来たら跳ね返す
				shot->Reversal();

				if (!RAudio::IsPlaying("Parry"))
				{
					RAudio::Play("Parry", 0.8f);
				}

				player.eventParry = false;
			}
			else {
				shot->SetIsAlive(false);
				player.DealDamage(shot->GetDamage());
			}
		}
	}

	for (auto& enemy : EnemyManager::GetInstance()->enemys)
	{
		if (enemy->enemyTag == BombSolider::GetEnemyTag()) {
			for (auto& shot : EnemyManager::GetInstance()->enemyShots)
			{
				//パリィした弾が当たったらダメージ
				if (ColPrimitive3D::CheckSphereToSphere(shot->collider, enemy->collider) &&
					shot->GetIsReversal()) {
					shot->SetIsAlive(false);

					RAudio::Play("Hit");

					//enemyにダメージ
					Vector3 knockVec = enemy->GetPos() - shot->GetPos();
					knockVec.Normalize();
					knockVec.y = 0;
					enemy->DealDamage(player.GetAttackPower(),
						knockVec, &player.obj);
				}
			}
		}
	}

	player.Update();
	Boss::GetInstance()->Update();
	WaveManager::Get()->Update();
	Level::Get()->Update();

	//敵同士の押し戻し
	for (auto& enemy1 : EnemyManager::GetInstance()->enemys)
	{
		//未出現なら処理しない
		if (!enemy1->GetIsSpawn())continue;

		//回収中なら処理しない
		if (enemy1->GetState() == EnemyCollect::GetStateStr())continue;

		//攻撃中と回収され中はこの判定をなくす
		if (enemy1->GetAttackState() != EnemyNowAttackState::GetStateStr())
		{
			//プレイヤーと判定、当たってるなら押し戻す
			if (ColPrimitive3D::CheckSphereToSphere(enemy1->collider, player.collider))
			{
				Vector3 repulsionVec = player.GetPos() - enemy1->GetPos();
				repulsionVec.Normalize();
				repulsionVec.y = 0;

				//一旦これだけ無理やり足す
				player.obj.mTransform.position += repulsionVec;

				//コライダーがもう一度当たらないようにコライダー更新
				player.UpdateCollider();
			}
		}

		for (auto& enemy2 : EnemyManager::GetInstance()->enemys)
		{
			//同個体なら処理しない
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
				if (enemy2->GetAttackState() == EnemyNormalState::GetStateStr())
				{
					enemy2->BehaviorOrigenPosPlus(e2RepulsionVec);
				}

				//コライダーがもう一度当たらないようにコライダー更新
				enemy1->UpdateCollider();
				enemy2->UpdateCollider();
			}
		}
	}

	//ボスの落下攻撃との当たり判定
	if (Boss::GetInstance()->fallAtkTimer.GetRun())
	{
		for (size_t i = 0; i < Boss::GetInstance()->fallParts.size(); i++)
		{
			if (ColPrimitive3D::CheckSphereToSphere(Boss::GetInstance()->fallParts[i].collider, player.collider))
			{
				player.DealDamage(Boss::GetInstance()->fallAtkPower);
			}
		}
	}

	//プレイヤーを押し戻す(ボス)
	if (ColPrimitive3D::CheckSphereToSphere(Boss::GetInstance()->collider, player.collider) && Boss::GetInstance()->isAppearanced) {
		Vector3 repulsionVec = player.GetPos() - Boss::GetInstance()->GetPos();
		repulsionVec.Normalize();
		repulsionVec.y = 0;

		//一旦これだけ無理やり足す
		player.obj.mTransform.position += repulsionVec;

		//コライダーがもう一度当たらないようにコライダー更新
		player.UpdateCollider();
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

	SpotLightManager::GetInstance()->Imgui();
	SpotLightManager::GetInstance()->Update();

	light.SetAmbientColor(ambient);
	light.Update();

	skydome.TransferBuffer(Camera::sNowCamera->mViewProjection);

	//F6かメニューボタン押されたらリザルトシーンへ
	if (RInput::GetInstance()->GetKeyDown(DIK_F6) ||
		RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_START))
	{
		RAudio::Stop("Boss");
		RAudio::Stop("Normal");
		SceneManager::GetInstance()->Change<FailedScene, WaxSceneTransition>();
	}

	controlUI.Update();
	bossAppTimerUI.Imgui();

	bossAppTimerUI.Update();
	waveUI.Update();

#pragma region ImGui
	if (RImGui::showImGui)
	{
		ImGui::SetNextWindowSize({ 400, 200 }, ImGuiCond_FirstUseEver);

		// デバッグモード //
		ImGui::Begin("デバッグ");
		ImGui::Text("デバッグモード中はシーン切り替えが発生しません");
		ImGui::Text("デバッグモードはF5で切り替えもできます");
		ImGui::Checkbox("parryTutorial", &player.parryTutorial);
		if (ImGui::Checkbox("デバッグモード切り替え", &Util::debugBool)) {
			ImGui::Text("デバッグモード中です");
		}
		if (ImGui::Button("セーブ")) {
			Parameter::Begin("DebugBool");
			Parameter::Save("debugBool", Util::debugBool);
			Parameter::End();
		}

		ImGui::End();

		ImGui::SetNextWindowSize({ 400, 200 }, ImGuiCond_FirstUseEver);

		// 平行光源 //
		ImGui::Begin("平行光源");

		ImGui::DragFloat3("アンビエント", &ambient.x);

		if (ImGui::Button("セーブ")) {
			Parameter::Begin("DirLight");
			Parameter::Save("ambient_R", ambient.x);
			Parameter::Save("ambient_G", ambient.y);
			Parameter::Save("ambient_B", ambient.z);
			Parameter::End();
		}

		ImGui::End();
	}

	SpawnDataLoader::OrderCreateGUI();

#pragma endregion
}

void ProtoScene::Draw()
{
	SpotLightManager::GetInstance()->Draw();

	//Minimap::GetInstance()->Draw();
	skydome.Draw();
	ParticleManager::GetInstance()->Draw();
	WaxManager::GetInstance()->Draw();
	//CollectPartManager::GetInstance()->Draw();

	Level::Get()->Draw();
	Boss::GetInstance()->Draw();
	player.Draw();

	EventCaller::Draw();

	//なんのイベントも呼ばれていないならUIを描画
	if (EventCaller::GetNowEventStr() == "") {
		controlUI.Draw();

		if (Boss::GetInstance()->isAppearanced == false)
		{
			//bossAppTimerUI.Draw();
			waveUI.Draw();
		}
	}

	//更新
	InstantDrawer::AllUpdate();
	InstantDrawer::AllDraw2D();

	SceneTrance::GetInstance()->Draw();
}

//void ProtoScene::MinimapCameraUpdate()
//{
//	Vector3 mmCameraVec = { 0, 0, 1 };
//	//カメラアングル適応
//	mmCameraVec *= Quaternion::AngleAxis(Vector3(1, 0, 0).Cross(mmCameraVec), 0.f);
//	mmCameraVec *= Quaternion::AngleAxis(Vector3(0, 1, 0).Cross(mmCameraVec), Util::AngleToRadian(89.9f));
//	//カメラの距離適応
//	mmCameraVec *= GameCamera::GetInstance()->mmCameraDist;
//
//	//アスペクト比1:1に
//	minimapCamera.mViewProjection.mAspect = 1.f / 1.f;
//
//	minimapCamera.mViewProjection.mEye = mmCameraVec;
//
//	minimapCamera.mViewProjection.mTarget = Vector3::ZERO;
//	minimapCamera.mViewProjection.UpdateMatrix();
//}
