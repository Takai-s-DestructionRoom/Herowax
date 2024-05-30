#include "Player.h"
#include "WaxManager.h"
#include "RInput.h"
#include "Camera.h"
#include "TimeManager.h"
#include "ParticleManager.h"
#include "Util.h"
#include "Quaternion.h"
#include "RImGui.h"
#include "Parameter.h"
#include "InstantDrawer.h"
#include "Renderer.h"
#include "boss.h"
#include "BossPart.h"
#include "Level.h"
#include "BossDeadState.h"
#include "TitleScene.h"
#include "SceneManager.h"
#include "SimpleSceneTransition.h"
#include "RAudio.h"
#include "EnemyManager.h"
#include "EventCaller.h"
#include "FailedScene.h"
#include "GameCamera.h"
#include <SimpleDrawer.h>

Player::Player() :GameObject(),
moveSpeed(1.f), moveAccelAmount(0.05f), isGround(true), hp(0), maxHP(10.f),
isJumping(false), jumpTimer(0.2f), jumpHeight(0.f), maxJumpHeight(5.f), jumpPower(2.f), jumpSpeed(0.f),
isAttack(false), atkSize(3.f), atkPower(1),
atkCoolTimer(0.3f), atkTimer(0.5f), atkHeight(1.f), solidTimer(5.f),
isFireStock(false), isWaxStock(true), waxCollectAmount(0)
{
	std::map<std::string, std::string> extract = Parameter::Extract("Player");
	moveSpeed = Parameter::GetParam(extract, "移動速度", 1.f);
	moveAccelAmount = Parameter::GetParam(extract, "移動加速度", 0.05f);
	gravity = Parameter::GetParam(extract, "重力", 0.098f);
	jumpPower = Parameter::GetParam(extract, "ジャンプ力", 2.0f);
	atkTimer.maxTime_ = Parameter::GetParam(extract, "攻撃時間", 0.5f);
	atkHeight = Parameter::GetParam(extract, "射出高度", 1.f);
	atkSize = Parameter::GetParam(extract, "攻撃範囲", 3.f);
	atkCoolTimer.maxTime_ = Parameter::GetParam(extract, "クールタイム", 0.3f);
	solidTimer.maxTime_ = Parameter::GetParam(extract, "固まるまでの時間", 5.f);
	atkPower = (int32_t)Parameter::GetParam(extract, "敵に与えるダメージ", 10.0f);

	pabloSideRange = Parameter::GetParam(extract, "パブロ攻撃の横の広がり", 5.f);
	pabloSpeedMag = Parameter::GetParam(extract, "パブロ攻撃時の移動速度低下係数", 0.2f);
	pabloShotSpeedMag = Parameter::GetParam(extract, "パブロ攻撃を移動しながら撃った時の係数", 2.0f);
	shotDeadZone = Parameter::GetParam(extract, "ショットが出る基準", 0.5f);

	initPos.x = Parameter::GetParam(extract, "初期座標X", 0.f);
	initPos.y = Parameter::GetParam(extract, "初期座標Y", 0.f);
	initPos.z = Parameter::GetParam(extract, "初期座標Z", 0.f);
	initRot.x = Parameter::GetParam(extract, "初期方向X", 0.f);
	initRot.y = Parameter::GetParam(extract, "初期方向Y", 0.f);
	initRot.z = Parameter::GetParam(extract, "初期方向Z", 0.f);
	attackHitCollider.r = Parameter::GetParam(extract, "敵がこの範囲に入ると攻撃状態へ遷移する大きさ", 1.0f);

	collectRangeModel = ModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube", true));
	waxCollectRange = Parameter::GetParam(extract, "ロウ回収範囲", 5.f);
	collectRangeModel.mTuneMaterial.mColor.a = Parameter::GetParam(extract, "範囲objの透明度", 0.5f);
	collectRangeModel.mTuneMaterial.mAmbient = Vector3(1, 1, 1) * 100.f;
	collectRangeModel.mTuneMaterial.mDiffuse = Vector3::ZERO;
	collectRangeModel.mTuneMaterial.mSpecular = Vector3::ZERO;

	waxCollectVertical = Parameter::GetParam(extract, "ロウ回収縦幅", 1000.f);

	attackState = std::make_unique<PlayerNormal>();

	attackDrawerObj = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));

	godmodeTimer.maxTime_ = Parameter::GetParam(extract, "無敵時間", 10.f);
	maxHP = Parameter::GetParam(extract, "最大HP", 10.0f);

	minRange = Parameter::GetParam(extract, "攻撃範囲_最小", minRange);
	maxRange = Parameter::GetParam(extract, "攻撃範囲_最大", maxRange);

	bagScale = Parameter::GetParam(extract, "風船の大きさ", 1.0f);
	humanOffset = Parameter::GetParam(extract, "人の位置Y", humanOffset);
	humanScale = Parameter::GetParam(extract, "人の大きさ", humanScale);
	collectScale = Parameter::GetParam(extract, "回収中の大きさ", collectScale);

	waxWall.parryTimer.maxTime_ = Parameter::GetParam(extract, "パリィの猶予時間", 0.1f);

	initWaxStock = (int32_t)Parameter::GetParam(extract, "ロウの初期最大ストック数", (float)initWaxStock);
	maxWaxStock = initWaxStock;
	waxStock = initWaxStock;
}

void Player::Init()
{
	waxUI.Init();

	Model::Load("./Resources/Model/collect/collect.obj", "collect", true);
	Model::Load("./Resources/Model/playerHuman/playerHuman.obj", "playerHuman", true);
	Model::Load("./Resources/Model/playerBag/playerBag.obj", "playerBag", true);
	Model::Load("./Resources/Model/Sphere.obj", "TankWater", true);

	RAudio::Load("Resources/Sounds/SE/P_attack.wav", "Attack");
	RAudio::Load("Resources/Sounds/SE/P_attackHit.wav", "Hit");
	RAudio::Load("Resources/Sounds/SE/P_enemyCollect.wav", "eCollect");

	RAudio::Load("Resources/Sounds/SE/playerShield.wav", "PShield");



	obj = PaintableModelObj("playerBag");
	humanObj = ModelObj("playerHuman");
	tankWaterObj = ModelObj("TankWater");
	tankMeterObj = ModelObj("TankWater");

	std::map<std::string, std::string> extract = Parameter::Extract("Player");
	defColor.r = Parameter::GetParam(extract, "プレイヤーの色R", 1);
	defColor.g = Parameter::GetParam(extract, "プレイヤーの色G", 1);
	defColor.b = Parameter::GetParam(extract, "プレイヤーの色B", 1);
	defColor.a = 1;

	obj.mTuneMaterial.mColor = defColor;

	hp = maxHP;
	//fireUnit.Init();
	isGodmode = false;
	godmodeTimer.Reset();

	redTimer_.nowTime_ = kGamingTimer_;
	redTimer_.Reset();
	greenTimer_.Reset();
	blueTimer_.Reset();

	redTimer_.SetEnd(true);
	greenTimer_.SetReverseEnd(true);
	blueTimer_.SetReverseEnd(true);

	//初期値適用
	obj.mTransform.position = initPos;

	Vector3 rot;
	rot.x = Util::AngleToRadian(initRot.x);
	rot.y = Util::AngleToRadian(initRot.y);
	rot.z = Util::AngleToRadian(initRot.z);
	obj.mTransform.rotation = rot;
	obj.mTransform.UpdateMatrix();

	isMove = true;

	waxWall.Init();
}

void Player::Reset()
{
	oldRot = rotVec;

	oldHp = hp;
	//回転初期化
	rotVec = { 0,0,0 };
}

void Player::Update()
{
	//タイマー更新
	damageCoolTimer.Update();
	godmodeTimer.Update();

	//カメラの向いてる方向算出
	cameraDir = Camera::sNowCamera->mViewProjection.mTarget - Camera::sNowCamera->mViewProjection.mEye;
	cameraDir.Normalize();
	cameraDir.y = 0;

	//ダメージ時点滅
	//DamageBlink();

	//回避していない時
	if (!avoidTimer.GetRun()) {
		//パッド接続してたら
		if (RInput::GetInstance()->GetPadConnect())
		{
			if (isMove)
			{
				MovePad();
			}
		}
		else
		{
			if (isMove)
			{
				MoveKey();
			}
		}
	}

	if (isMove)
	{
		Rotation();

		attackState->Update(this);
		//前のステートと異なれば
		if (changingState) {
			//ステートを変化させる
			std::swap(attackState, nextState);
			changingState = false;
			nextState = nullptr;
		}

		////回避
		//Avoidance();
	}

	//攻撃ボタン入力中で、実際にロウが出せたら攻撃フラグを立てる
	isAttack = (RInput::GetInstance()->GetRTrigger() || RInput::GetKey(DIK_SPACE)) &&
		(waxStock > 0) && WaxManager::GetInstance()->notCollect;

	//-----------クールタイム管理-----------//
	atkTimer.Update();
	atkCoolTimer.Update();

	//攻撃終わったらクールタイム開始
	if (atkTimer.GetEnd())
	{
		atkCoolTimer.Start();
		atkTimer.Reset();
	}

	//クールタイム終わったら攻撃再びできるように
	if (atkCoolTimer.GetEnd())
	{
		atkCoolTimer.Reset();
	}

	//---ロウ回収処理周り---///
	//if (isWaxStock == false)
	//{
	//	waxStock = maxWaxStock;
	//}
	////ストックがおかしな値にならないように
	//waxStock = Util::Clamp(waxStock, 0, maxWaxStock);

	//回収処理
	WaxCollect();

	maxWaxStock = Util::Clamp(maxWaxStock, 0, 100);
	waxStock = Util::Clamp(waxStock, 0, maxWaxStock);

	WaxManager::GetInstance()->SetMaxWaxStock(maxWaxStock);

	//回収が終わったらモデルを戻す
	if (WaxManager::GetInstance()->notCollect && modelChange) {
		obj.mModel = ModelManager::Get("playerBag");
		modelChange = false;
	}
	//ジャンプなくなったので、地面座標にピッタリくっつける
	obj.mTransform.position.y = Level::Get()->ground->GetTransform().position.y;

	//地面に埋ってたら
	//if (obj.mTransform.position.y < Level::Get()->ground.mTransform.position.y)
	//{
	//	//地面に触れるとこまで移動
	//	obj.mTransform.position.y = Level::Get()->ground.mTransform.position.y;

	//	if (isGround == false)
	//	{
	//		isGround = true;

	//		//エミッターの座標はプレイヤーの座標からY座標だけにスケール分ずらしたもの
	//		Vector3 emitterPos = GetCenterPos();

	//		ParticleManager::GetInstance()->AddRing(
	//			emitterPos, "player_landing_ring");
	//	}
	//	isJumping = false;
	//}

	//HP0になったら死ぬ
	if (hp <= 0)
	{
		//死んだ瞬間なら遷移を呼ぶ
		if (isAlive && !Util::debugBool) {
			//シーン遷移
			SceneManager::GetInstance()->Change<FailedScene, SimpleSceneTransition>();
		}
		isAlive = false;
	}

	//ダメージクールタイム中なら色を変える
	if (damageCoolTimer.GetStarted()) {
		brightColor.r = Easing::OutQuad(1.0f, 0.0f, damageCoolTimer.GetTimeRate());
	}

	//のけぞり中ならのけぞらせる
	backwardTimer.Update();
	if (backwardTimer.GetStarted()) {
		float radStartX = Util::AngleToRadian(-30.0f);
		rotVec.x = Easing::InQuad(radStartX, 0, backwardTimer.GetTimeRate());
	}

	//回転を適用
	obj.mTransform.rotation = rotVec;
	//風船のモデルを180度回転(元の向きが正面のため)
	obj.mTransform.rotation.y += Util::AngleToRadian(180.f);

	//無敵モードなら
	if (isGodmode)
	{
		if (godmodeTimer.GetStarted() == false)
		{
			godmodeTimer.Start();
		}

		////ゲーミング色に
		//obj.mTuneMaterial.mColor = GamingColorUpdate();
	}

	//無敵時間終了したらフラグもfalseに
	if (godmodeTimer.GetEnd())
	{
		isGodmode = false;
		obj.mTuneMaterial.mColor = defColor;

		godmodeTimer.Reset();
	}

	moveVec *=
		moveSpeed * moveAccel *
		WaxManager::GetInstance()->notCollect;				//移動速度をかけ合わせたら完成(回収中は動けない)
	obj.mTransform.position += moveVec;						//完成したものを座標に足し合わせる

	//なんかによってスケールをいじる
	if (!WaxManager::GetInstance()->notCollect)
	{
		float wave = 0.2f * Util::GetRatio(-1.0f, 1.0f, sinf(Util::PI * 5 * waxCollectingTime));
		obj.mTransform.scale = { bagScale + wave, bagScale + wave, bagScale + wave };
	}
	else
	{
		obj.mTransform.scale = Vector3(1, 1, 1) * bagScale;
	}

	//攻撃中と回収中なら正面へ、それ以外なら後ろへ
	if (isAttack || !WaxManager::GetInstance()->notCollect) {
		Vector3 tVec = collectRangeModel.mTransform.position - collectCol.start;
		tVec.y = 0;
		tVec.Normalize();
		obj.mTransform.rotation = Quaternion::LookAt(tVec).ToEuler();
	}

	//上に乗ってる人型の位置を合わせる
	humanObj.mTransform = obj.mTransform;
	humanObj.mTransform.position.y += humanOffset;

	//向きを合わせる
	humanObj.mTransform.rotation = obj.mTransform.rotation;
	humanObj.mTransform.rotation.y -= Util::AngleToRadian(180.f);

	//大きさを適用
	humanObj.mTransform.scale = Vector3(1, 1, 1) * humanScale;

	UpdateCollider();
	UpdateAttackCollider();

	bool isCollision = false;
	float len = 0;
	int32_t count = 0;

	//移動制限(フェンス準拠)
	for (auto& wall : Level::Get()->wallCol)
	{
		if (ColPrimitive3D::CheckSphereToPlane(collider, wall))
		{
			Vector3 vec;	//壁にぶつかった際に移動させるベクトル

			// 進行方向ベクトルと壁の法線ベクトルに垂直なベクトルを算出
			vec = moveVec.Cross(-wall.normal);

			// 算出したベクトルと壁の法線ベクトルに垂直なベクトルを算出、これが
			// 元の移動成分から壁方向の移動成分を抜いたベクトル
			vec = -wall.normal.Cross(vec);

			if (isCollision)
			{
				//2個目の壁なら移動前の壁の距離を一旦取り、
				len = abs(wall.distance - Vector2(obj.mTransform.position.x, obj.mTransform.position.z).Length());
				//「移動前の2個目の壁との距離」の方が「移動後の1個目の壁との距離」より遠ければ2個目の壁基準で動かす
				if (len >= toWallLen)
				{
					//slideVec = vec;
					slideVec = wall.normal.Cross(slideVec);
				}
			}
			else
			{
				//1個目の壁なら仮に動いた後の壁との距離を保存する
				toWallLen = abs(wall.distance - Vector2(obj.mTransform.position.x - vec.x, obj.mTransform.position.z - vec.z).Length());
				slideVec = vec;
				isCollision = true;
			}
			count++;
		}
	}

	if (RImGui::showImGui)
	{
		ImGui::SetNextWindowSize({ 600, 250 }, ImGuiCond_FirstUseEver);

		ImGui::Begin("移動制限");
		ImGui::Text("残った方向:%f,%f,%f", slideVec.x, slideVec.y, slideVec.z);
		ImGui::Text("壁との距離:%f", toWallLen);
		ImGui::Text("2枚目壁との距離:%f", len);
		ImGui::Text("何枚の壁と衝突してるか:%d", count);

		ImGui::End();
	}

	toWallLen = 0;

	if (isCollision)
	{
		moveVec = -moveVec;
		obj.mTransform.position += moveVec + slideVec;
	}

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	BrightTransferBuffer(Camera::sNowCamera->mViewProjection);

	humanObj.mTransform.UpdateMatrix();
	humanObj.TransferBuffer(Camera::sNowCamera->mViewProjection);

	//タンクデータ
	tankWaterObj.mTransform.position = tankMeterObj.mTransform.position = obj.mTransform.position + Vector3(0, 1.0f + bagScale / 2.0f, 0);
	tankWaterObj.mTransform.scale = obj.mTransform.scale - Vector3(0.8f, 0.8f, 0.8f);
	tankMeterObj.mTransform.scale = obj.mTransform.scale - Vector3(0.7f, 0.7f, 0.7f);
	tankWaterObj.mTransform.UpdateMatrix();
	tankWaterObj.mTuneMaterial.mAmbient = { 10, 10, 10 };
	tankWaterObj.mTuneMaterial.mDiffuse = { 10, 10, 10 };
	tankWaterObj.TransferBuffer(Camera::sNowCamera->mViewProjection);
	tankMeterObj.mTuneMaterial.mColor = { 0, 0, 1, 1 };
	tankMeterObj.mTuneMaterial.mAmbient = { 100, 100, 100 };
	tankMeterObj.mTransform.UpdateMatrix();
	tankMeterObj.TransferBuffer(Camera::sNowCamera->mViewProjection);
	tankBuff->centerPos = tankMeterBuff->centerPos = tankWaterObj.mTransform.position;
	tankBuff->amplitude = 0.04f;
	tankBuff->frequency = 10.0f;
	float tankRatio = waxStock / 100.0f;
	float newTankValue = -(tankWaterObj.mTransform.scale.y + tankBuff->amplitude)
		+ (tankWaterObj.mTransform.scale.y * 2.0f + tankBuff->amplitude) * tankRatio;
	tankValue += (newTankValue - tankValue) / 7.0f;
	tankBuff->upper = tankValue;
	tankBuff->time += TimeManager::deltaTime;
	tankMeterBuff->upper = -tankMeterObj.mTransform.scale.y
		+ (tankMeterObj.mTransform.scale.y * 2.0f) * (maxWaxStock / 100.0f);
	tankMeterBuff->thickness = 0.05f;

	if (RInput::GetKeyDown(DIK_H)) {
		waxStock = 100;
	}

	ui.Update(this);

	waxUI.Update(obj.mTransform.position);

	ShieldUp();

	//残像
	/*for (auto& once : afterimagesObj)
	{
		once->Update();
	}*/

#pragma region ImGui
	if (RImGui::showImGui)
	{
		ImGui::SetNextWindowSize({ 600, 250 }, ImGuiCond_FirstUseEver);

		ImGui::Begin("Player");

		ImGui::Checkbox("吸収フラグ", &WaxManager::GetInstance()->notCollect);
		ImGui::Text("現在のHP:%f", hp);
		ImGui::InputFloat("最大HP:", &maxHP, 1.0f);
		ImGui::Text("Lスティック移動、Aボタンジャンプ、Rで攻撃,Lでロウ回収");
		ImGui::Text("WASD移動、スペースジャンプ、右クリで攻撃,Pでパブロ攻撃,Qでロウ回収");

		if (ImGui::TreeNode("初期状態設定"))
		{
			ImGui::SliderFloat("初期座標X", &initPos.x, -100.f, 100.f);
			ImGui::SliderFloat("初期座標Y", &initPos.y, -100.f, 100.f);
			ImGui::SliderFloat("初期座標Z", &initPos.z, -100.f, 100.f);
			ImGui::SliderFloat("初期方向X", &initRot.x, 0.f, 360.f);
			ImGui::SliderFloat("初期方向Y", &initRot.y, 0.f, 360.f);
			ImGui::SliderFloat("初期方向Z", &initRot.z, 0.f, 360.f);
			ImGui::InputInt("ロウの初期最大ストック数", &initWaxStock, 1, 100);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Transform系")) {

			ImGui::DragFloat("人の位置Y", &humanOffset);
			ImGui::DragFloat("人の大きさ", &humanScale);
			ImGui::DragFloat("風船の大きさ", &bagScale);
			ImGui::DragFloat("回収中の大きさ", &collectScale);
			ImGui::ColorEdit4("プレイヤーの色", &obj.mTuneMaterial.mColor.r);
			ImGui::DragFloat("モデルをずらすY", &modelOffset.y, 0.1f);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("移動系"))
		{
			ImGui::Text("座標:%f,%f,%f", GetPos().x, GetPos().y, GetPos().z);
			ImGui::Text("動く方向:%f,%f,%f", moveVec.x, moveVec.y, moveVec.z);
			ImGui::Text("ジャンプの高さ:%f", jumpHeight);
			ImGui::Text("ジャンプ速度:%f", jumpSpeed);
			ImGui::Text("加速度:%f", moveAccel);
			ImGui::SliderFloat("移動速度:%f", &moveSpeed, 0.f, 5.f);
			ImGui::SliderFloat("移動加速度:%f", &moveAccelAmount, 0.f, 0.1f);
			ImGui::SliderFloat("重力:%f", &gravity, 0.f, 0.2f);
			ImGui::SliderFloat("ジャンプ力:%f", &jumpPower, 0.f, 5.f);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("攻撃系"))
		{
			ImGui::Text("攻撃中か:%d", isAttack);
			ImGui::Checkbox("ロウをストック性にするか", &isWaxStock);

			ImGui::InputInt("敵に与えるダメージ", &atkPower, 1);
			ImGui::DragFloat("攻撃範囲_横", &pabloSideRange, 0.1f);
			ImGui::DragFloat("攻撃範囲_最小", &minRange, 0.1f);
			ImGui::DragFloat("攻撃範囲_最大", &maxRange, 0.1f);
			ImGui::DragFloat("射出高度", &atkHeight, 0.1f);
			ImGui::DragFloat("攻撃が地面につくまでの時間", &atkTimer.maxTime_, 0.1f);
			ImGui::DragFloat("クールタイム", &atkCoolTimer.maxTime_, 0.1f);
			ImGui::InputInt("一度に出るロウの数", &waxNum, 1);
			ImGui::InputInt("ロウの最大ストック数", &maxWaxStock, 1, 100);
			ImGui::Text("現在のロウのストック数:%d", waxStock);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("ロウ回収系"))
		{
			ImGui::SliderFloat("ロウ回収範囲(横幅)", &waxCollectRange, 0.f, 100.f);
			ImGui::SliderFloat("範囲objの透明度", &collectRangeModel.mTuneMaterial.mColor.a, 0.f, 1.f);
			ImGui::InputFloat("ロウ回収範囲(縦幅)", &waxCollectVertical, 1.f);
			ImGui::InputFloat("ロウ回収の時間", &WaxManager::GetInstance()->collectTime, 1.f);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("当たり判定"))
		{
			ImGui::Checkbox("当たり判定描画切り替え", &isDrawCollider);
			ImGui::InputFloat("敵がこの範囲に入ると攻撃状態へ遷移する大きさ", &attackHitCollider.r, 1.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("ガード系"))
		{
			ImGui::DragFloat("パリィの猶予時間", &waxWall.parryTimer.maxTime_, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("MUTEKI系"))
		{
			ImGui::Checkbox("無敵状態切り替え", &isGodmode);
			ImGui::InputFloat("無敵時間", &godmodeTimer.maxTime_, 1.0f);
			ImGui::TreePop();
		}

		if (ImGui::Button("Reset")) {
			Init();
		}
		if (ImGui::Button("セーブ")) {
			Parameter::Begin("Player");
			Parameter::Save("移動速度", moveSpeed);
			Parameter::Save("移動加速度", moveAccelAmount);
			Parameter::Save("重力", gravity);
			Parameter::Save("ジャンプ力", jumpPower);
			Parameter::Save("敵に与えるダメージ", (float)atkPower);
			Parameter::Save("攻撃時間", atkTimer.maxTime_);
			Parameter::Save("射出高度", atkHeight);
			Parameter::Save("攻撃範囲", atkSize);
			Parameter::Save("クールタイム", atkCoolTimer.maxTime_);
			Parameter::Save("固まるまでの時間", solidTimer.maxTime_);
			Parameter::Save("パブロ攻撃の横の広がり", pabloSideRange);
			Parameter::Save("パブロ攻撃時の移動速度低下係数", pabloSpeedMag);
			Parameter::Save("パブロ攻撃を移動しながら撃った時の係数", pabloShotSpeedMag);
			Parameter::Save("ショットが出る基準", shotDeadZone);
			Parameter::Save("初期座標X", initPos.x);
			Parameter::Save("初期座標Y", initPos.y);
			Parameter::Save("初期座標Z", initPos.z);
			Parameter::Save("初期方向X", initRot.x);
			Parameter::Save("初期方向Y", initRot.y);
			Parameter::Save("初期方向Z", initRot.z);
			Parameter::Save("ロウ回収範囲", waxCollectRange);
			Parameter::Save("範囲objの透明度", collectRangeModel.mTuneMaterial.mColor.a);
			Parameter::Save("敵がこの範囲に入ると攻撃状態へ遷移する大きさ", attackHitCollider.r);
			Parameter::Save("ロウ回収縦幅", waxCollectVertical);
			Parameter::Save("プレイヤーの色R", obj.mTuneMaterial.mColor.r);
			Parameter::Save("プレイヤーの色G", obj.mTuneMaterial.mColor.g);
			Parameter::Save("プレイヤーの色B", obj.mTuneMaterial.mColor.b);
			Parameter::Save("無敵時間", godmodeTimer.maxTime_);
			Parameter::Save("最大HP", maxHP);
			Parameter::Save("攻撃範囲_最小", minRange);
			Parameter::Save("攻撃範囲_最大", maxRange);
			Parameter::Save("人の位置Y", humanOffset);
			Parameter::Save("人の大きさ", humanScale);
			Parameter::Save("風船の大きさ", bagScale);
			Parameter::Save("回収中の大きさ", collectScale);
			Parameter::Save("ロウの初期最大ストック数", initWaxStock);
			Parameter::Save("パリィの猶予時間", waxWall.parryTimer.maxTime_);
			Parameter::End();
		}

		ImGui::End();
	}
#pragma endregion
}

void Player::Draw()
{
	if (isAlive || Util::debugBool)
	{
		/*for (auto& once : afterimagesObj)
		{
			once->Draw();
		}*/

		BrightDrawTrans();

		//タンク内描画
		{
			for (RenderOrder order : tankWaterObj.GetRenderOrder()) {
				RenderOrder orderA = order;
				orderA.mRootSignature = GetTankWaterRootSig()->mPtr.Get();
				orderA.pipelineState = GetTankWaterPipeline()->mPtr.Get();
				orderA.rootData.push_back(RootData(RootDataType::SRBUFFER_CBV, tankBuff.mBuff));

				RenderOrder orderB = orderA;
				orderB.pipelineState = GetTankWaterPipelineB()->mPtr.Get();
				Renderer::DrawCall("Opaque", orderA);
				Renderer::DrawCall("Opaque", orderB);
			}

			if (maxWaxStock < 100) {
				for (RenderOrder order : tankMeterObj.GetRenderOrder()) {
					order.mRootSignature = GetTankMeterRootSig()->mPtr.Get();
					order.pipelineState = GetTankMeterPipeline()->mPtr.Get();
					order.rootData.push_back(RootData(RootDataType::SRBUFFER_CBV, tankMeterBuff.mBuff));
					Renderer::DrawCall("Opaque", order);
				}
			}
		}

		//回収中は別モデルに置き換えるので描画しない
		if (WaxManager::GetInstance()->notCollect) {
			humanObj.Draw();
		}

		collectRangeModel.Draw();

		//なんのイベントも呼ばれていないならUIを描画
		if (EventCaller::GetNowEventStr() == "") {
			ui.Draw();
		}

		DrawAttackCollider();

		waxUI.Draw();
		waxWall.Draw();
	}
}

void Player::MovePad()
{
	Vector2 stick = RInput::GetInstance()->GetPadLStick();

	//スティックが倒されてたら
	if (stick.LengthSq() > 0.f) {
		//カメラから注視点へのベクトル
		Vector3 cameraVec = Camera::sNowCamera->mViewProjection.mTarget - Camera::sNowCamera->mViewProjection.mEye;
		//カメラの角度
		float cameraRad = atan2f(cameraVec.x, cameraVec.z);
		//スティックの角度
		float stickRad = atan2f(stick.x, stick.y);

		moveVec = { 0, 0, 1 };									//正面を基準に
		moveVec *= Matrix4::RotationY(cameraRad + stickRad);	//カメラの角度から更にスティックの入力角度を足して
		moveVec.Normalize();									//方向だけの情報なので正規化して
		moveVec *= stick.LengthSq();							//傾き具合を大きさに反映

		//入力中は加速度足し続ける
		moveAccel += moveAccelAmount;

		//エミッターの座標はプレイヤーの座標から移動方向の逆側にスケール分ずらしたもの
		Vector3 emitterPos = obj.mTransform.position;
		Vector2 mVelo = { -moveVec.x,-moveVec.z };	//moveVecを正規化すると実際の移動に支障が出るので一時変数に格納
		mVelo.Normalize();
		emitterPos.x += mVelo.x * obj.mTransform.scale.x;
		emitterPos.z += mVelo.y * obj.mTransform.scale.z;
		emitterPos.y += obj.mTransform.scale.y;

		ParticleManager::GetInstance()->AddSimple(
			emitterPos, "player_move");
	}
	else
	{
		moveAccel -= moveAccelAmount;	//入力されてなければ徐々に減速
	}

	moveAccel = Util::Clamp(moveAccel, 0.f, 1.f);			//無限に増減しないよう抑える

	////接地してて回収中じゃない時にAボタン押すと
	//if (isGround && RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A) &&
	//	WaxManager::GetInstance()->isCollected)
	//{
	//	isJumping = true;
	//	isGround = false;
	//	jumpSpeed = jumpPower;	//速度に初速を代入
	//	jumpTimer.Start();

	//	//エミッターの座標はプレイヤーの座標からY座標だけにスケール分ずらしたもの
	//	Vector3 emitterPos = GetCenterPos();

	//	ParticleManager::GetInstance()->AddRing(
	//		emitterPos, "player_jump_ring");
	//}

	////ジャンプ中は
	//if (isJumping) {
	//	jumpTimer.Update();
	//	//速度に対して重力をかけ続けて減速
	//	jumpSpeed -= gravity;
	//	//高さに対して速度を足し続ける
	//	jumpHeight += jumpSpeed;
	//}
	//else
	//{
	//	//ジャンプしてないときはジャンプの高さを0に
	//	jumpHeight = 0.f;
	//}

	//「ジャンプの高さ」+「プレイヤーの大きさ」を反映
	//obj.mTransform.position.y = jumpHeight + obj.mTransform.scale.y;
}

void Player::MoveKey()
{
	Vector2 keyVec = {};
	keyVec.x = (float)(RInput::GetInstance()->GetKey(DIK_D) - RInput::GetInstance()->GetKey(DIK_A));
	keyVec.y = (float)(RInput::GetInstance()->GetKey(DIK_W) - RInput::GetInstance()->GetKey(DIK_S));

	//キー入力されてて回収中じゃないなら
	if (keyVec.LengthSq() > 0.f && WaxManager::GetInstance()->notCollect) {
		//カメラから注視点へのベクトル
		Vector3 cameraVec = Camera::sNowCamera->mViewProjection.mTarget - Camera::sNowCamera->mViewProjection.mEye;
		//カメラの角度
		float cameraRad = atan2f(cameraVec.x, cameraVec.z);
		//キーの角度(?)
		float keyRad = atan2f(keyVec.x, keyVec.y);

		moveVec = { 0, 0, 1 };									//正面を基準に
		moveVec *= Matrix4::RotationY(cameraRad + keyRad);		//カメラの角度から更にキーの入力角度を足して
		moveVec.Normalize();									//方向だけの情報なので正規化して

		//入力中は加速度足し続ける
		moveAccel += moveAccelAmount;
	}
	else
	{
		moveAccel -= moveAccelAmount;	//入力されてなければ徐々に減速
	}

	moveAccel = Util::Clamp(moveAccel, 0.f, moveVec.LengthSq());

	////接地時で回収中じゃない時にスペース押すと
	//if (isGround && RInput::GetInstance()->GetKeyDown(DIK_SPACE) &&
	//	WaxManager::GetInstance()->isCollected)
	//{
	//	isJumping = true;
	//	isGround = false;
	//	jumpSpeed = jumpPower;	//速度に初速を代入
	//	jumpTimer.Start();

	//	//エミッターの座標はプレイヤーの座標からY座標だけにスケール分ずらしたもの
	//	Vector3 emitterPos = GetCenterPos();

	//	ParticleManager::GetInstance()->AddRing(
	//		emitterPos, "player_jump_ring");
	//}

	////ジャンプ中は
	//if (isJumping) {
	//	jumpTimer.Update();
	//	//速度に対して重力をかけ続けて減速
	//	jumpSpeed -= gravity;
	//	//高さに対して速度を足し続ける
	//	jumpHeight += jumpSpeed;
	//}
	//else
	//{
	//	//ジャンプしてないときはジャンプの高さを0に
	//	jumpHeight = 0.f;
	//}

	if (keyVec.LengthSq() > 0.f || isJumping) {
		//エミッターの座標はプレイヤーの座標から移動方向の逆側にスケール分ずらしたもの
		Vector3 emitterPos = obj.mTransform.position;
		Vector2 mVelo = { -moveVec.x,-moveVec.z };	//moveVecを正規化すると実際の移動に支障が出るので一時変数に格納
		mVelo.Normalize();
		emitterPos.x += mVelo.x * obj.mTransform.scale.x;
		emitterPos.z += mVelo.y * obj.mTransform.scale.z;
		emitterPos.y += obj.mTransform.scale.y;

		ParticleManager::GetInstance()->AddSimple(
			emitterPos, "player_move");
	}

	//「ジャンプの高さ」+「プレイヤーの大きさ」を反映
	//obj.mTransform.position.y = jumpHeight + obj.mTransform.scale.y;
}

void Player::Avoidance()
{
	avoidTimer.Update();
	avoidConsumTimer.Update();

	////削除
	//for (auto itr = afterimagesObj.begin(); itr != afterimagesObj.end();)
	//{
	//	if (!(*itr)->isAlive)
	//	{
	//		itr = afterimagesObj.erase(itr);
	//	}
	//	else {
	//		itr++;
	//	}
	//}

	//ボタンを押したら
	if (!avoidTimer.GetRun() &&
		(RInput::GetPadButtonDown(XINPUT_GAMEPAD_A) ||
			RInput::GetKeyDown(DIK_X)))
	{
		avoidVec = -GetFrontVec();
		avoidTimer.Start();
		avoidConsumTimer.maxTime_ = avoidTimer.maxTime_ / (float)avoidConsumWax;
		avoidConsumTimer.Reset();
		maxAvoidSpeed = moveSpeed / 2;
		minAvoidSpeed = maxAvoidSpeed / 2.5f;
	}

	//回避中は方向を足し続ける
	if (avoidTimer.GetRun() && waxStock > 0) {
		//スピードを最初を早めに、最後を遅めに
		avoidSpeed = Easing::OutQuad(maxAvoidSpeed, minAvoidSpeed, avoidTimer.GetTimeRate());

		moveAccel += moveAccelAmount;
		moveAccel = Util::Clamp(moveAccel, 0.f, 1.f);			//無限に増減しないよう抑える

		moveVec += avoidVec * avoidSpeed;

		ParticleManager::GetInstance()->AddSimple(GetFootPos(), "collect_smoke_avoid");

		//ロウを一定間隔で消費
		if (!avoidConsumTimer.GetRun()) {
			avoidConsumTimer.Start();
			////ストック減らす
			//waxStock--;

			/*WaxManager::GetInstance()->Create(obj.mTransform, obj.mTransform.position, atkHeight,
				atkSize, atkTimer.maxTime_, solidTimer.maxTime_);*/
		}

		/*afterimagesObj.emplace_back();
		afterimagesObj.back() = std::make_unique<AfterImage>();
		afterimagesObj.back()->Init();
		afterimagesObj.back()->obj.mModel = obj.mModel;
		afterimagesObj.back()->obj.mTransform = obj.mTransform;
		afterimagesObj.back()->obj.mTuneMaterial.mColor = { 1,1,1,1 };
		afterimagesObj.back()->brightColor = brightColor;*/
	}
	else {
		avoidVec = { 0,0,0 };
	}
}

void Player::Rotation()
{
	Vector2 RStick = RInput::GetInstance()->GetRStick(false, true);

	bool change = false;

	//Rスティック入力があったら
	if (RStick.LengthSq() > 0.0f) {
		//カメラから注視点へのベクトル
		Vector3 cameraVec = Camera::sNowCamera->mViewProjection.mTarget -
			Camera::sNowCamera->mViewProjection.mEye;
		cameraVec.y = 0;
		cameraVec.Normalize();

		//ターゲットの方向を向いてくれる
		Quaternion aLookat = Quaternion::LookAt(cameraVec);

		//euler軸へ変換
		rotVec = aLookat.ToEuler();
		change = true;
	}

	Vector2 LStick = RInput::GetInstance()->GetLStick(true, false);
	//スティック入力されてて回収中じゃなければ
	if (LStick.LengthSq() > 0 && WaxManager::GetInstance()->notCollect) {
		//カメラから注視点へのベクトル
		Vector3 cameraVec = Camera::sNowCamera->mViewProjection.mTarget -
			Camera::sNowCamera->mViewProjection.mEye;
		cameraVec.y = 0;
		cameraVec.Normalize();
		//カメラの角度
		float cameraRad = atan2f(cameraVec.x, cameraVec.z);
		//スティックの角度
		float stickRad = atan2f(LStick.x, LStick.y);

		Vector3 shotVec = { 0,0,1 };								//正面を基準に
		shotVec *= Matrix4::RotationY(cameraRad + stickRad);	//カメラの角度から更にスティックの入力角度を足して
		shotVec.Normalize();									//方向だけの情報なので正規化して
		shotVec *= LStick.LengthSq();							//傾き具合を大きさに反映

		//ターゲットの方向を向いてくれる
		Quaternion aLookat = Quaternion::LookAt(shotVec);

		//euler軸へ変換
		rotVec = aLookat.ToEuler();
		change = true;
	}
	if (!change) {
		rotVec = oldRot;
	}
}

void Player::PabloAttack()
{
	//攻撃中かストックないなら次の攻撃が出せない
	if (atkCoolTimer.GetRun() || waxStock <= 0)return;
	atkCoolTimer.Start();
	soundFlag = false;

	Vector3 pabloVec = { 0,0,0 };
	//入力があるならそっちへ
	pabloVec = collectRangeModel.mTransform.position - collectCol.start;
	pabloVec.y = 0;
	pabloVec.Normalize();

	atkVec = pabloVec;

	Vector2 rotaVec = { pabloVec.x,pabloVec.z };
	rotaVec = rotaVec.Rotation(-Util::PI / 2);

	//pabloVecの横ベクトルを取る
	Vector3 sidePabloVec = { rotaVec.x,0,rotaVec.y };
	sidePabloVec.Normalize();

	//発射数の半分(切り捨て)はマイナス横ベクトル方向へずらす

	//発射数分ロウを生成、座標を生成するたびプラス横ベクトル方向へずらす
	//座標を生成するたびプラス正面ベクトル方向へずらす
	Vector3 sideRandMin = -sidePabloVec * pabloSideRange;
	Vector3 sideRandMax = sidePabloVec * pabloSideRange;

	//もしminの方が大きくなってしまっていたら入れ替える
	if (sideRandMin.x > sideRandMax.x) {
		float save = sideRandMin.x;
		sideRandMin.x = sideRandMax.x;
		sideRandMax.x = save;
	}

	if (sideRandMin.z > sideRandMax.z) {
		float save = sideRandMin.z;
		sideRandMin.z = sideRandMax.z;
		sideRandMax.z = save;
	}

	for (int32_t i = 0; i < waxNum; i++)
	{
		//ロウのストックが0になったら出ない
		if (waxStock <= 0)break;
		//ストック減らす
		waxStock--;

		Transform spawnTrans = obj.mTransform;
		//横のランダムを決定
		spawnTrans.position.x += Util::GetRand(sideRandMin.x, sideRandMax.x);
		spawnTrans.position.z += Util::GetRand(sideRandMin.z, sideRandMax.z);

		//最低値と最大値を元に終点を決定
		float randRange = Util::GetRand(minRange, maxRange);

		Vector3 endpos = spawnTrans.position + pabloVec * randRange;
		endpos.y = 0;	//yは地面座標(今は決め打ち0)

		WaxManager::GetInstance()->Create(spawnTrans, endpos, atkHeight,
			atkSize, atkTimer.maxTime_, solidTimer.maxTime_);
	}

	//音鳴らす
	RAudio::Play("Attack", 0.6f);
}

void Player::WaxCollect()
{
	//トランスフォームはプレイヤー基準に
	collectRangeModel.mTransform = obj.mTransform;
	collectRangeModel.mTransform.scale = { waxCollectRange,0.1f,waxCollectVertical };

	//当たり判定で使うレイの設定
	//当たり判定で使うレイの設定
	Vector3 dir = Camera::sNowCamera->mViewProjection.mTarget - Camera::sNowCamera->mViewProjection.mEye;
	dir.y = 0;
	collectCol.dir = dir.Normalize();

	//プレイヤーよりちょい後ろも回収範囲に含む
	collectCol.start = GetFootPos() - collectCol.dir * obj.mTransform.scale.z * 1.5f;
	collectCol.radius = waxCollectRange * 0.5f + obj.mTransform.scale.z * 1.5f;

	//大きさ分前に置く
	collectRangeModel.mTransform.position += collectCol.dir * waxCollectVertical * 0.5f;

	//回転
	Quaternion lookat = Quaternion::LookAt(collectCol.dir);
	collectRangeModel.mTransform.rotation = lookat.ToEuler();

	//更新
	collectRangeModel.mTransform.UpdateMatrix();
	collectRangeModel.TransferBuffer(Camera::sNowCamera->mViewProjection);

	//イベント中でなければ入る
	if (EventCaller::GetNowEventStr() == "") {

		int32_t nowPlusNum = EnemyManager::GetInstance()->collectNum;
		if (nowPlusNum > 0) {
			waxCollectAmount += nowPlusNum;
			MaxWaxPlus(nowPlusNum);
		}
		EnemyManager::GetInstance()->collectNum = 0;

		//回収が完了したら増やす
		waxCollectAmount += WaxManager::GetInstance()->collectWaxNum;
		WaxManager::GetInstance()->collectWaxNum = 0;

		if (waxCollectAmount > 0)
		{
			waxStock += waxCollectAmount;

			collectCount += waxCollectAmount;

			//音鳴らす
			RAudio::Play("eCollect", 0.5f, 1.0f + 1.0f * (collectCount / 100.0f));
			
			ParticleManager::GetInstance()->AddHoming2D(
				Util::GetScreenPos(obj.mTransform.position), { 150.f,150.f }, waxCollectAmount, 0.8f,
				Color::kWaxColor,
				TextureManager::Load("./Resources/Particle/particle_simple.png", "particleSimple"),
				150.f, 180.f, { -1.f,-1.f }, { 1.f,1.f },
				ui.numDrawer.GetPos(), 0.f,
				0.01f, 0.05f,
				0.1f, 0.f);

			waxCollectAmount = 0;
		}
	}

	if (GetWaxCollectButtonDown()) {
		WaxManager::GetInstance()->notCollect = false;
		waxCollectingTime = 0;
	
		if (!RAudio::IsPlaying("Collect"))
		{
			RAudio::Play("Collect", 0.6f);
		}
	}

	if (isMove)
	{
		//回収ボタンポチーw
		if (!WaxManager::GetInstance()->notCollect)
		{
			obj.mModel = ModelManager::Get("collect");
			modelChange = true;

			//ロウがストック性かつ地面についてて回収できる状態なら
			if (isWaxStock && isGround)
			{
				//ロウ回収
				WaxManager::GetInstance()->Collect(collectCol, waxCollectVertical, this);
			}
			//腕吸収
			if (boss->parts[(int32_t)PartsNum::LeftHand].isCollected &&
				!boss->parts[(int32_t)PartsNum::LeftHand].collectTimer.GetStarted()) {
				if (RayToSphereCol(collectCol, boss->parts[(int32_t)PartsNum::LeftHand].collider))
				{
					//今のロウとの距離
					float len = (collectCol.start -
						boss->parts[(int32_t)PartsNum::LeftHand].GetPos()).Length();

					//見たロウが範囲外ならスキップ
					if (waxCollectVertical >= len) {
						//とりあえず壊しちゃう
						boss->parts[(int32_t)PartsNum::LeftHand].collectPos = collectCol.start;
						boss->parts[(int32_t)PartsNum::LeftHand].ChangeState<BossPartCollect>();
						waxCollectAmount += 5;
						MaxWaxPlus(5);
					}
				}
			}
			if (boss->parts[(int32_t)PartsNum::RightHand].isCollected &&
				!boss->parts[(int32_t)PartsNum::RightHand].collectTimer.GetStarted()) {
				if (ColPrimitive3D::RayToSphereCol(collectCol, boss->parts[(int32_t)PartsNum::RightHand].collider))
				{
					//今のロウとの距離
					float len = (collectCol.start -
						boss->parts[(int32_t)PartsNum::RightHand].GetPos()).Length();

					//見たロウが範囲外ならスキップ
					if (waxCollectVertical >= len) {
						//とりあえず壊しちゃう
						boss->parts[(int32_t)PartsNum::RightHand].collectPos = collectCol.start;
						boss->parts[(int32_t)PartsNum::RightHand].ChangeState<BossPartCollect>();
						//腕の吸収値も変数化したい
						waxCollectAmount += 5;
						MaxWaxPlus(5);
					}
				}
			}

			for (auto& enemy : EnemyManager::GetInstance()->enemys)
			{
				//回収ボタン押されたときに固まってるなら吸収
				if (enemy->GetIsSolid() && ColPrimitive3D::RayToSphereCol(collectCol, enemy->collider))
				{
					//回収状態に遷移
					WaxManager::GetInstance()->notCollect = false;
					EnemyManager::GetInstance()->collectTarget = this;
					enemy->isCollect = true;
					enemy->ChangeState<EnemyCollect>();
				}
			}
		}
	}

	//キーから手を離したら動ける
	if ((RInput::GetInstance()->GetLTriggerUp() ||
		RInput::GetInstance()->GetKeyUp(DIK_Q))) {

		WaxManager::GetInstance()->notCollect = true;

		RAudio::Stop("Collect");
	}

	//回収中にダメージを受けたら回収をキャンセル
	if (!WaxManager::GetInstance()->notCollect && GetDamageTrigger()) {

		WaxManager::GetInstance()->notCollect = true;

		//回収したロウと同じ数のロウを地面にばらまく
		WaxLeakOut(collectCount, -10.f, 10.f);
	}

	if (!WaxManager::GetInstance()->notCollect) {
		//ロウからターゲットを除去
		WaxManager::GetInstance()->collectTarget = this;
		EnemyManager::GetInstance()->collectTarget = this;
	}

	if (WaxManager::GetInstance()->notCollect) {
		collectCount = 0;
		//ロウからターゲットを除去
		WaxManager::GetInstance()->collectTarget = nullptr;
		EnemyManager::GetInstance()->collectTarget = nullptr;
	}

	if (WaxManager::GetInstance()->notCollect == false)
	{
		Vector3 emitPos = obj.mTransform.position + dir * (waxCollectVertical - waxCollectRange);
		emitPos.y += obj.mTransform.scale.y;
		ParticleManager::GetInstance()->AddHoming(emitPos, "collect_smoke_homing", GetFootPos());

		static int32_t count = 0;
		if (count % 3 == 0)
		{
			ParticleManager::GetInstance()->AddHoming(emitPos, "collect_homing", GetFootPos());
		}

		Vector3 emitPosFront = obj.mTransform.position + dir * waxCollectRange;
		emitPosFront.y += obj.mTransform.scale.y;
		ParticleManager::GetInstance()->AddHoming(emitPosFront, "front_collect_smoke_homing", GetFootPos());

		count++;
		waxCollectingTime += TimeManager::deltaTime;
	}
}

Vector3 Player::GetFootPos()
{
	Vector3 result;

	result = obj.mTransform.position;
	result.y += obj.mTransform.scale.y;
	return result;
}

void Player::DealDamage(float damage)
{
	//ダメージクールタイム中か無敵モードならダメージが与えられない
	if (damageCoolTimer.GetRun() || isGodmode)return;

	damageCoolTimer.Start();
	blinkTimer.Start();
	RAudio::Play("Hit");
	hp -= damage;

	//パーティクル生成
	ParticleManager::GetInstance()->AddSimple(obj.mTransform.position, "player_hit");

	//ちょっとのけぞる
	//モーション遷移
	backwardTimer.maxTime_ = damageCoolTimer.maxTime_ / 2;
	backwardTimer.Start();

	float shakeValue = damage;
	shakeValue = max(shakeValue, 1.0f);//最低1.0f
	GameCamera::GetInstance()->Shake(0.15f, shakeValue);
}

void Player::MaxWaxPlus(int32_t plus)
{
	maxWaxStock += plus;

	waxUI.Start();
}

void Player::WaxLeakOut(int32_t leakNum, float minLeakLength, float maxLeakLength)
{
	for (int32_t i = 0; i < leakNum; i++)
	{
		if (waxStock > 0)
		{
			waxStock -= 1;

			Transform startTrans = waxWall.obj.mTransform;
			startTrans.position.y = atkHeight;

			Vector3 endPos = waxWall.obj.mTransform.position;
			endPos.x += Util::GetRand(minLeakLength, maxLeakLength);
			endPos.z += Util::GetRand(minLeakLength, maxLeakLength);

			WaxManager::GetInstance()->Create(
				startTrans,
				endPos,
				atkHeight,
				atkSize,
				atkTimer.maxTime_,
				solidTimer.maxTime_);
		}
	}
}

Color Player::GamingColorUpdate()
{
	//タイマー更新
	redTimer_.Update();
	greenTimer_.Update();
	blueTimer_.Update();

	//タイマー無限ループ
	if (blueTimer_.GetReverseEnd() && !greenTimer_.GetStarted())
	{
		greenTimer_.Start();
	}

	if (greenTimer_.GetEnd() && !redTimer_.GetReverseStarted())
	{
		redTimer_.ReverseStart();
	}

	if (redTimer_.GetReverseEnd() && !blueTimer_.GetStarted())
	{
		blueTimer_.Start();
	}

	if (blueTimer_.GetEnd() && !greenTimer_.GetReverseStarted())
	{
		greenTimer_.ReverseStart();
	}

	if (greenTimer_.GetReverseEnd() && !redTimer_.GetStarted())
	{
		redTimer_.Start();
	}

	if (redTimer_.GetEnd() && !blueTimer_.GetReverseStarted())
	{
		blueTimer_.ReverseStart();
	}

	Color gamingColor(redTimer_.GetTimeRate(), greenTimer_.GetTimeRate(), blueTimer_.GetTimeRate());

	//若干暗めにしつつ
	gamingColor = gamingColor * 0.7f;
	//人間が見た時の見え方が違うからRGBごとに調整
	gamingColor.r *= 0.8f;
	gamingColor.g *= 0.6f;
	gamingColor.b *= 1.2f;
	return gamingColor;
}

void Player::DamageBlink()
{
	blinkTimer.RoopReverse();

	blinkTimer.maxTime_ = damageCoolTimer.maxTime_ / (blinkNum * 2);

	//ダメージ処理中なら
	if (damageCoolTimer.GetRun()) {
		obj.mTuneMaterial.mColor.a = (1.0f - blinkTimer.GetTimeRate()) + 0.2f;
	}
	else {
		obj.mTuneMaterial.mColor.a = 1.f;
	}
}

void Player::ShieldUp()
{
	//盾の位置をプレイヤーの正面へ
	Vector3 frontVec = Camera::sNowCamera->mViewProjection.mTarget - Camera::sNowCamera->mViewProjection.mEye;
	frontVec.y = 0;
	frontVec.Normalize();
	frontVec *= 10.0f;

	/*waxWall.obj.mTransform.position = obj.mTransform.position + frontVec;
	waxWall.obj.mTransform.rotation = Quaternion::LookAt(frontVec).ToEuler();
	waxWall.obj.mTransform.rotation.y += Util::AngleToRadian(-90.f);*/

	waxWall.obj.mTransform.position = obj.mTransform.position;

	if (RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_X) ||
		RInput::GetKeyDown(DIK_Z) && WaxManager::GetInstance()->notCollect) {
		//パリィ状態でなければ出現
		if (!waxWall.GetParry()) {
			if (waxWall.StartCheck(waxStock)) {

				WaxLeakOut(waxWall.START_CHECK_WAXNUM);

				waxWall.Start();

				RAudio::Play("PShield", 0.8f);
			}
		}
	}

	if (waxWall.GetParry() || waxWall.GetLeakOutMode())
	{
		//毎フレーム放してるかチェック
		if (RInput::GetInstance()->GetPadButtonUp(XINPUT_GAMEPAD_X) || RInput::GetKeyUp(DIK_Z)) {
			//離したら終了
			waxWall.End();
		}
	}

	//ロウ漏れモードなら
	if (waxWall.GetLeakOutMode()) {
		//ロウが残っているかチェック
		if (waxWall.ContinueCheck(waxStock)) {

			WaxLeakOut(waxWall.CONTINUE_CHECK_WAXNUM);
		}
	}

	waxWall.Update();
}

void Player::UpdateAttackCollider()
{
	attackHitCollider.pos = GetPos();

	attackDrawerObj.mTuneMaterial.mColor = { 1,1,0,1 };
	attackDrawerObj.mTransform.position = attackHitCollider.pos;
	float r = attackHitCollider.r;
	attackDrawerObj.mTransform.scale = { r,r,r };

	attackDrawerObj.mTransform.UpdateMatrix();
	attackDrawerObj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Player::DrawAttackCollider()
{
	//描画しないならスキップ
	if (!isDrawCollider)return;

	//パイプラインをワイヤーフレームに
	PipelineStateDesc pipedesc = RDirectX::GetDefPipeline().mDesc;
	pipedesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

	GraphicsPipeline pipe = GraphicsPipeline::GetOrCreate("WireObject", pipedesc);
	for (RenderOrder& order : attackDrawerObj.GetRenderOrder()) {
		order.pipelineState = pipe.mPtr.Get();
		Renderer::DrawCall("Opaque", order);
	}
}

bool Player::GetWaxCollectButtonDown()
{
	return (RInput::GetInstance()->GetLTriggerDown() ||
		RInput::GetInstance()->GetKeyDown(DIK_Q));
}

void AfterImage::Init()
{
	lifeTimer.Start();
	isAlive = true;
}

void AfterImage::Update()
{
	lifeTimer.Update();
	if (lifeTimer.GetEnd()) {
		isAlive = false;
	}

	obj.mTuneMaterial.mColor.a = Easing::OutQuad(0.0f, 1.0f, lifeTimer.GetTimeRate());

	if (obj.mTuneMaterial.mColor.a > 0.5f) {
		int32_t hoge;
		hoge = 100;
	}

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void AfterImage::Draw()
{
	if (isAlive) {
		obj.Draw("Transparent");
	}
}

RootSignature* Player::GetTankWaterRootSig()
{
	RootSignatureDesc desc = RDirectX::GetDefRootSignature().mDesc;

	desc.RootParamaters.emplace_back();

	desc.RootParamaters.back().ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.RootParamaters.back().Descriptor.ShaderRegister = 10;
	desc.RootParamaters.back().Descriptor.RegisterSpace = 0;
	desc.RootParamaters.back().ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	return &RootSignature::GetOrCreate("TankWater", desc);
}

GraphicsPipeline* Player::GetTankWaterPipeline()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().mDesc;
	desc.pRootSignature = GetTankWaterRootSig()->mPtr.Get();
	desc.PS = Shader::GetOrCreate("TankWaterPS", "./Shader/TankWater/TankWaterPS.hlsl", "main", "ps_5_1");

	return &GraphicsPipeline::GetOrCreate("TankWater", desc);
}

GraphicsPipeline* Player::GetTankWaterPipelineB()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().mDesc;
	desc.pRootSignature = GetTankWaterRootSig()->mPtr.Get();
	desc.VS = Shader::GetOrCreate("TankWaterVS", "./Shader/TankWater/TankWaterBackVS.hlsl", "main", "vs_5_1");
	desc.PS = Shader::GetOrCreate("TankWaterPS", "./Shader/TankWater/TankWaterPS.hlsl", "main", "ps_5_1");

	desc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
	return &GraphicsPipeline::GetOrCreate("TankWaterBack", desc);
}

RootSignature* Player::GetTankMeterRootSig()
{
	RootSignatureDesc desc = RDirectX::GetDefRootSignature().mDesc;

	desc.RootParamaters.emplace_back();

	desc.RootParamaters.back().ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.RootParamaters.back().Descriptor.ShaderRegister = 10;
	desc.RootParamaters.back().Descriptor.RegisterSpace = 0;
	desc.RootParamaters.back().ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	return &RootSignature::GetOrCreate("TankMeter", desc);
}

GraphicsPipeline* Player::GetTankMeterPipeline()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().mDesc;
	desc.pRootSignature = GetTankWaterRootSig()->mPtr.Get();
	desc.PS = Shader::GetOrCreate("TankMeterPS", "./Shader/TankWater/TankMeterPS.hlsl", "main", "ps_5_1");

	desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	return &GraphicsPipeline::GetOrCreate("TankMeter", desc);
}