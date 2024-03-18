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

ProtoScene::ProtoScene()
{
	//ブラシの読み込み
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

	level.Load();
}

void ProtoScene::Init()
{
	Camera::sNowCamera = &camera;

	std::map<std::string, std::string> extract = Parameter::Extract("Camera");
	cameraDist = Parameter::GetParam(extract,"カメラ距離", -20.f);
	cameraAngle.x = Parameter::GetParam(extract,"カメラアングルX", Util::AngleToRadian(20.f));
	cameraAngle.y = 0.f;
	cameraSpeed = Parameter::GetParam(extract,"カメラの移動速度", 0.01f);

	extract = Parameter::Extract("Paint");
	paintSize = Parameter::GetParam(extract, "ペイントサイズ", 300.f);
	paintColor.r = Parameter::GetParam(extract, "ペイントカラー_R", 1.f);
	paintColor.g = Parameter::GetParam(extract, "ペイントカラー_G", 1.f);
	paintColor.b = Parameter::GetParam(extract, "ペイントカラー_B", 1.f);
	paintColor.a = Parameter::GetParam(extract, "ペイントカラー_A", 1.f);

	LightGroup::sNowLight = &light;

	player.Init();

	ParticleManager::GetInstance()->Init();

	WaxManager::GetInstance()->Init();
	FireManager::GetInstance()->Init();
	TemperatureManager::GetInstance()->Init();

	//とりあえず最初のステージを設定しておく
	level.Extract("test");

	//地面を設定
	EnemyManager::GetInstance()->SetGround(level.GetGround());
	//タワーを入れる
	EnemyManager::GetInstance()->SetTarget(&level.GetTower()->obj);
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
		if (ColPrimitive3D::CheckSphereToSphere(enemy.collider, level.GetTower()->collider)) {
			enemy.SetDeath();
			Vector3 vec = level.GetTower()->GetPos() - enemy.GetPos();
			level.GetTower()->Damage(1.f,vec);
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
						enemy.trappedWaxGroup = group.get();
						
						//enemyにダメージ
						Vector3 knockVec = player.atkVec;
						knockVec.y = 0;
						enemy.DealDamage(WaxManager::GetInstance()->waxDamage,
							knockVec, &player.obj);
						//お試し実装:自分が攻撃を当てた相手が自分を追いかけてくる
						if (player.GetTauntMode()) {
							enemy.SetTarget(&player.obj);
						}
					}
					//地面に付いた蝋に当たった時は蝋足止め状態へ遷移
					else {
						enemy.ChangeState<EnemySlow>();
						enemy.trappedWaxGroup = group.get();

						//そのロウが燃えているなら一緒に燃える
						if (wax->GetState() != "Normal")
						{
							enemy.ChangeState<EnemyBurning>();
						}
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
	EnemyManager::GetInstance()->Update();

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
	
	//塗る処理(いったん全検索のカス)
	for (auto& group : WaxManager::GetInstance()->waxGroups)
	{
		for (auto& wax : group->waxs)
		{
			//地面に着弾したロウとは判定を行いたくないのでスキップ
			if (wax->isGround) continue;

			for (auto& paintObj : level.objects)
			{
				//ここで大雑把に当たり判定を取って、絶対に当たってないやつを除外する
				if (!ColPrimitive3D::CheckSphereToSphere(wax->collider, paintObj->collider))continue;

				for (auto& paintTri : paintObj->obj.GetTriangle())
				{
					Vector3 closestPoint = { 0,0,0 };
					//当たったトライアングルがあるならそこを塗る
					if (ColPrimitive3D::CheckSphereToTriangle(wax->collider, paintTri, &closestPoint)) {
						PaintableInfo info;

						//交点と球の中心で必ず当たるレイを作り、交点に塗る
						Vector3 posA = wax->collider.pos;
						Vector3 posB = closestPoint;
						Vector3 dir = (posB - posA).Normalize();

						ColPrimitive3D::Ray ray = { posA,dir };

						if (paintObj->obj.GetInfo(ray, &info))
						{
							//情報が完全一致するなら塗る
							if (info.tri == paintTri) {
								paintObj->obj.Paint(info.closestPos, info.hitMeshIndex, info.hitIndex,
									"brush", paintColor, Vector2(paintSize, paintSize), camera.mViewProjection.mMatrix);

								//塗ったロウはハカイ
								wax->SetIsAlive(false);
							}
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
					if (enemy.trappedWaxGroup == group2.get()) {
						enemy.trappedWaxGroup = group1.get();
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

	ImGui::SetNextWindowSize({ 300, 150 });
	// ペイント //
	ImGui::Begin("ペイント", NULL, window_flags);
	ImGui::SliderFloat("ペイントサイズ", &paintSize,0.0f,300.f);
	ImGui::ColorEdit4("PaintColor", &paintColor.r);

	if (ImGui::Button("セーブ")) {
		Parameter::Begin("Paint");
		Parameter::Save("ペイントサイズ", paintSize);
		Parameter::Save("ペイントカラー_R", paintColor.r);
		Parameter::Save("ペイントカラー_G", paintColor.g);
		Parameter::Save("ペイントカラー_B", paintColor.b);
		Parameter::Save("ペイントカラー_A", paintColor.a);
		Parameter::End();
	}
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

	EnemyManager::GetInstance()->Draw();

	//更新
	InstantDrawer::AllUpdate();
	InstantDrawer::AllDraw2D();
}
