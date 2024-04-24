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

Player::Player() :GameObject(),
moveSpeed(1.f), moveAccelAmount(0.05f), isGround(true), hp(0), maxHP(10.f),
isJumping(false), jumpTimer(0.2f), jumpHeight(0.f), maxJumpHeight(5.f), jumpPower(2.f), jumpSpeed(0.f),
isAttack(false), atkSize(3.f), atkPower(1),
atkCoolTimer(0.3f), atkTimer(0.5f), atkHeight(1.f), solidTimer(5.f),
isFireStock(false), isWaxStock(true), isCollectFan(false), maxWaxStock(20)
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
	collectRangeModel.mTuneMaterial.mAmbient = Vector3::ONE * 100.f;
	collectRangeModel.mTuneMaterial.mDiffuse = Vector3::ZERO;
	collectRangeModel.mTuneMaterial.mSpecular = Vector3::ZERO;

	collectRangeModelCircle = ModelObj(Model::Load("./Resources/Model/wax/wax.obj", "Wax", true));
	waxCollectDist = Parameter::GetParam(extract, "ロウ回収半径", 5.f);
	waxCollectAngle = Parameter::GetParam(extract, "ロウ回収角度", 90.f);
	waxCollectVertical = Parameter::GetParam(extract, "ロウ回収縦幅", 1000.f);
	collectRangeModelCircle.mTuneMaterial.mColor.a = Parameter::GetParam(extract, "範囲(扇)objの透明度", 0.5f);

	collectRangeModelRayLeft = ModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube", true));
	collectRangeModelRayRight = ModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube", true));

	attackState = std::make_unique<PlayerNormal>();

	attackDrawerObj = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));

	godmodeTimer.maxTime_ = Parameter::GetParam(extract, "無敵時間", 10.f);
	maxHP = Parameter::GetParam(extract, "最大HP", 10.0f);

	minRange = Parameter::GetParam(extract, "攻撃範囲_最小", minRange);
	maxRange = Parameter::GetParam(extract, "攻撃範囲_最大", maxRange);

	Model::Load("./Resources/Model/collect/collect.obj", "collect", true);
	Model::Load("./Resources/Model/playerHuman/playerHuman.obj", "playerHuman", true);
	Model::Load("./Resources/Model/playerBag/playerBag.obj", "playerBag", true);

	bagScale = Parameter::GetParam(extract, "風船の大きさ", 1.0f);
	humanOffset = Parameter::GetParam(extract,"人の位置Y", humanOffset);
	humanScale = Parameter::GetParam(extract,"人の大きさ", humanScale);
	collectScale = Parameter::GetParam(extract,"回収中の大きさ", collectScale);

	RAudio::Load("Resources/Sounds/SE/P_attack.wav", "Attack");
	
	
	RAudio::Load("Resources/Sounds/SE/P_enemyCollect.wav", "eCollect");
}

void Player::Init()
{
	obj = PaintableModelObj("playerBag");
	obj.mPaintDissolveMapTex = TextureManager::Load("./Resources/DissolveMap.png",
		"DissolveMap");

	humanObj = ModelObj("playerHuman");

	waxTankObj = PaintableModelObj(Model::Load("./Resources/Model/sphere.obj", "sphere", true));
	waxTankObj.mPaintDissolveMapTex = TextureManager::Load("./Resources/deleteDesolveTex.png",
		"deleteDesolveTex");

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

	waxStock = maxWaxStock;

	//初期値適用
	obj.mTransform.position = initPos;

	Vector3 rot;
	rot.x = Util::AngleToRadian(initRot.x);
	rot.y = Util::AngleToRadian(initRot.y);
	rot.z = Util::AngleToRadian(initRot.z);
	obj.mTransform.rotation = rot;
	obj.mTransform.UpdateMatrix();

	isMove = true;
}

void Player::Reset()
{
	oldRot = rotVec;
	//回転初期化
	rotVec = { 0,0,0 };
}

void Player::Update()
{
	Reset();

	//タイマー更新
	damageCoolTimer.Update();
	godmodeTimer.Update();

	//カメラの向いてる方向算出
	cameraDir = Camera::sNowCamera->mViewProjection.mTarget - Camera::sNowCamera->mViewProjection.mEye;
	cameraDir.Normalize();
	cameraDir.y = 0;

	//ダメージ時点滅
	//DamageBlink();

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
	}

	//攻撃ボタン入力中で、実際にロウが出せたら攻撃フラグを立てる
	isAttack = (RInput::GetInstance()->GetRTrigger() || RInput::GetKey(DIK_SPACE)) && (waxStock > 0);

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
	//回収処理
	WaxCollect();

	//回収が終わったらモデルを戻す
	if (WaxManager::GetInstance()->isCollected && modelChange) {
		obj.mModel = ModelManager::Get("playerBag");
		modelChange = false;
	}

	if (isWaxStock == false)
	{
		waxStock = maxWaxStock;
	}
	//ストックがおかしな値にならないように
	waxStock = Util::Clamp(waxStock, 0, maxWaxStock);

	//ジャンプなくなったので、地面座標にピッタリくっつける
	obj.mTransform.position.y = Level::Get()->ground.mTransform.position.y;

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
			SceneManager::GetInstance()->Change<TitleScene, SimpleSceneTransition>();
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

		//ゲーミング色に
		obj.mTuneMaterial.mColor = GamingColorUpdate();
	}

	//無敵時間終了したらフラグもfalseに
	if (godmodeTimer.GetEnd())
	{
		isGodmode = false;
		obj.mTuneMaterial.mColor = defColor;

		godmodeTimer.Reset();
	}

	bool nowCollected = WaxManager::GetInstance()->isCollected && 
		!EnemyManager::GetInstance()->GetNowCollectEnemy();

	moveVec *=
		moveSpeed * moveAccel *
		nowCollected;				//移動速度をかけ合わせたら完成(回収中は動けない)
	obj.mTransform.position += moveVec;						//完成したものを座標に足し合わせる

	//回収中なら回収中モデルのスケールを入れる
	if (!WaxManager::GetInstance()->isCollected)
	{
		obj.mTransform.scale = Vector3::ONE * collectScale;
	}
	else
	{
		obj.mTransform.scale = Vector3::ONE * bagScale;
	}

	//攻撃中と回収中なら正面へ、それ以外なら後ろへ
	if (isAttack || !nowCollected) {
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
	humanObj.mTransform.scale = Vector3::ONE * humanScale;

	//
	waxTankObj.mTransform = obj.mTransform;
	waxTankObj.mTransform.scale = (Vector3::ONE * bagScale) * 0.7f;
	waxTankObj.mTransform.position.y = obj.mTransform.position.y + (bagScale * 0.75f);

	UpdateCollider();
	UpdateAttackCollider();

	bool isCollision = false;
	float len = 0;

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
					slideVec = vec;
				}
			}
			else
			{
				//1個目の壁なら仮に動いた後の壁との距離を保存する
				toWallLen = abs(wall.distance - Vector2(obj.mTransform.position.x - vec.x, obj.mTransform.position.z - vec.z).Length());
				slideVec = vec;
				isCollision = true;
			}
		}
	}

	ImGui::SetNextWindowSize({ 600, 250 }, ImGuiCond_FirstUseEver);

	ImGui::Begin("移動制限");
	ImGui::Text("残った方向:%f,%f,%f", slideVec.x, slideVec.y, slideVec.z);
	ImGui::Text("壁との距離:%f", toWallLen);
	ImGui::Text("2枚目壁との距離:%f", len);

	ImGui::End();

	toWallLen = 0;

	if (isCollision)
	{
		moveVec = -moveVec;
		obj.mTransform.position += moveVec + slideVec;
		obj.mTuneMaterial.mColor = Color::kPink;
	}
	else
	{
		obj.mTuneMaterial.mColor = Color::kWhite;
	}

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	obj.mPaintDataBuff->dissolveVal = (float)waxStock / (float)maxWaxStock;;
	obj.mPaintDataBuff->color = Color(0.8f, 0.6f, 0.35f, 1.0f);
	obj.mPaintDataBuff->slide += TimeManager::deltaTime;
	BrightTransferBuffer(Camera::sNowCamera->mViewProjection);

	humanObj.mTransform.UpdateMatrix();
	humanObj.TransferBuffer(Camera::sNowCamera->mViewProjection);

	waxTankObj.mTransform.UpdateMatrix();
	waxTankObj.mPaintDataBuff->dissolveVal = (float)waxStock / (float)maxWaxStock;;
	waxTankObj.mPaintDataBuff->color = Color(0.8f, 0.6f, 0.35f, 1.0f);
	waxTankObj.mPaintDataBuff->slide += TimeManager::deltaTime;
	waxTankObj.TransferBuffer(Camera::sNowCamera->mViewProjection);

	ui.Update(this);

#pragma region ImGui
	ImGui::SetNextWindowSize({ 600, 250 }, ImGuiCond_FirstUseEver);

	ImGui::Begin("Player");

	ImGui::Text("現在のHP:%f", hp);
	ImGui::InputFloat("最大HP:", &maxHP, 1.0f);
	ImGui::Text("Lスティック移動、Aボタンジャンプ、Rで攻撃,Lでロウ回収");
	ImGui::Text("WASD移動、スペースジャンプ、右クリで攻撃,Pでパブロ攻撃,Qでロウ回収");

	if (ImGui::TreeNode("Transform系")) {

		ImGui::DragFloat("人の位置Y", &humanOffset);
		ImGui::DragFloat("人の大きさ", &humanScale);
		ImGui::DragFloat("風船の大きさ", &bagScale);
		ImGui::DragFloat("回収中の大きさ", &collectScale);
		ImGui::ColorEdit4("プレイヤーの色", &obj.mTuneMaterial.mColor.r);
		ImGui::DragFloat("モデルをずらすY", &modelOffset.y,0.1f);

		if (ImGui::TreeNode("初期状態設定"))
		{
			ImGui::SliderFloat("初期座標X", &initPos.x, -100.f, 100.f);
			ImGui::SliderFloat("初期座標Y", &initPos.y, -100.f, 100.f);
			ImGui::SliderFloat("初期座標Z", &initPos.z, -100.f, 100.f);
			ImGui::SliderFloat("初期方向X", &initRot.x, 0.f, 360.f);
			ImGui::SliderFloat("初期方向Y", &initRot.y, 0.f, 360.f);
			ImGui::SliderFloat("初期方向Z", &initRot.z, 0.f, 360.f);

			ImGui::TreePop();
		}

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
		if (ImGui::TreeNode("扇"))
		{
			ImGui::SliderFloat("ロウ回収半径", &waxCollectDist, 0.f, 100.f);
			ImGui::SliderFloat("ロウ回収角度", &waxCollectAngle, 1.f, 180.f);
			ImGui::SliderFloat("範囲(扇)objの透明度", &collectRangeModelCircle.mTuneMaterial.mColor.a, 0.f, 1.f);
			ImGui::Text("回収できる状態か:%d", WaxManager::GetInstance()->isCollected);
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("当たり判定"))
	{
		ImGui::Checkbox("当たり判定描画切り替え", &isDrawCollider);
		ImGui::InputFloat("敵がこの範囲に入ると攻撃状態へ遷移する大きさ", &attackHitCollider.r, 1.0f);
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
		Parameter::Save("風船の大きさ",bagScale);
		Parameter::Save("回収中の大きさ", collectScale);

		Parameter::End();
	}

	ImGui::End();
#pragma endregion
}

void Player::Draw()
{
	if (isAlive || Util::debugBool)
	{
		//waxTankObj.Draw();

		BrightDraw();
		
		//回収中は別モデルに置き換えるので描画しない
		if (WaxManager::GetInstance()->isCollected) {
			humanObj.Draw();
		}

		if (isCollectFan)
		{
			collectRangeModelCircle.Draw();
			collectRangeModelRayLeft.Draw();
			collectRangeModelRayRight.Draw();
		}
		else
		{
			collectRangeModel.Draw();
		}
		ui.Draw();

		DrawAttackCollider();
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
	if (keyVec.LengthSq() > 0.f && WaxManager::GetInstance()->isCollected) {
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

		ParticleManager::GetInstance()->AddSimple(
			emitterPos, "player_move");
	}

	//「ジャンプの高さ」+「プレイヤーの大きさ」を反映
	//obj.mTransform.position.y = jumpHeight + obj.mTransform.scale.y;
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
	if (LStick.LengthSq() > 0 && WaxManager::GetInstance()->isCollected) {
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
	RAudio::Play("Attack");
}

void Player::WaxCollect()
{
	//扇の範囲表す用レイ
	collectRangeModelRayLeft.mTransform = obj.mTransform;
	collectRangeModelRayRight.mTransform = obj.mTransform;
	collectRangeModelRayLeft.mTransform.scale = { 0.1f,0.1f,waxCollectDist * 2.f };
	collectRangeModelRayRight.mTransform.scale = { 0.1f,0.1f,waxCollectDist * 2.f };
	collectRangeModelRayLeft.mTransform.rotation.y += Util::AngleToRadian(-waxCollectAngle * 0.5f);
	collectRangeModelRayRight.mTransform.rotation.y += Util::AngleToRadian(waxCollectAngle * 0.5f);
	
	collectRangeModelRayLeft.mTransform.UpdateMatrix();
	collectRangeModelRayLeft.TransferBuffer(Camera::sNowCamera->mViewProjection);
	collectRangeModelRayRight.mTransform.UpdateMatrix();
	collectRangeModelRayRight.TransferBuffer(Camera::sNowCamera->mViewProjection);

	//トランスフォームはプレイヤー基準に
	collectRangeModel.mTransform = obj.mTransform;
	collectRangeModel.mTransform.scale = { waxCollectRange,0.1f,waxCollectVertical };

	collectRangeModelCircle.mTransform = obj.mTransform;
	collectRangeModelCircle.mTransform.scale = { waxCollectDist,0.1f,waxCollectDist };

	//当たり判定で使うレイの設定
	//当たり判定で使うレイの設定
	Vector3 dir = Camera::sNowCamera->mViewProjection.mTarget - Camera::sNowCamera->mViewProjection.mEye;
	dir.y = 0;
	collectCol.dir = dir.Normalize();
	
	collectCol.start = GetFootPos();
	collectCol.radius = waxCollectRange * 0.5f;

	//大きさ分前に置く
	collectRangeModel.mTransform.position += collectCol.dir * waxCollectVertical * 0.5f;

	//回転
	Quaternion lookat = Quaternion::LookAt(collectCol.dir);
	collectRangeModel.mTransform.rotation = lookat.ToEuler();

	//更新
	collectRangeModel.mTransform.UpdateMatrix();
	collectRangeModel.TransferBuffer(Camera::sNowCamera->mViewProjection);

	collectRangeModelCircle.mTransform.UpdateMatrix();
	collectRangeModelCircle.TransferBuffer(Camera::sNowCamera->mViewProjection);

	//当たり判定で使う球の設定
	collectColFan.pos = GetFootPos();
	collectColFan.r = waxCollectDist;

	//イベント中でなければ入る
	if (EventCaller::GetNowEventStr() == "") {
		//回収したロウに応じてストック増やす
		if (isWaxStock && WaxManager::GetInstance()->isCollected)
		{
			if (waxCollectAmount > 0)
			{
				waxStock += waxCollectAmount;
				waxCollectAmount = 0;
				//音鳴らす
				RAudio::Play("eCollect");
			}

			//最大量を超えて回収してたら最大量を増やす
			if (waxStock > maxWaxStock)
			{
				maxWaxStock = waxStock;
				//音鳴らす
				RAudio::Play("eCollect");
			}
		}
	}

	if (isMove)
	{
		//回収ボタンポチーw
		if (GetWaxCollectButtonDown())
		{
			bool isCollectSuccess = false;;
			//ロウがストック性かつ地面についてて回収できる状態なら
			if (isWaxStock && isGround && WaxManager::GetInstance()->isCollected)
			{
				isCollectSuccess = true;

				if (isCollectFan)
				{
					//ロウ回収
					WaxManager::GetInstance()->CollectFan(collectColFan, GetFrontVec(), waxCollectAngle);
				}
				else
				{
					//ロウ回収
					waxCollectAmount += WaxManager::GetInstance()->Collect(collectCol, waxCollectVertical);
				}
			}
			//腕吸収
			if (boss->parts[(int32_t)PartsNum::LeftHand].isCollected) {
				if (RayToSphereCol(collectCol, boss->parts[(int32_t)PartsNum::LeftHand].collider))
				{
					isCollectSuccess = true;

					//今のロウとの距離
					float len = (collectCol.start -
						boss->parts[(int32_t)PartsNum::LeftHand].GetPos()).Length();

					//見たロウが範囲外ならスキップ
					if (waxCollectVertical >= len) {
						//とりあえず壊しちゃう
						boss->parts[(int32_t)PartsNum::LeftHand].collectPos = collectCol.start;
						boss->parts[(int32_t)PartsNum::LeftHand].ChangeState<BossPartCollect>();
						waxCollectAmount += 1;
					}
				}
			}
			if (boss->parts[(int32_t)PartsNum::RightHand].isCollected) {
				if (ColPrimitive3D::RayToSphereCol(collectCol, boss->parts[(int32_t)PartsNum::RightHand].collider))
				{
					//今のロウとの距離
					float len = (collectCol.start -
						boss->parts[(int32_t)PartsNum::RightHand].GetPos()).Length();

					//見たロウが範囲外ならスキップ
					if (waxCollectVertical >= len) {
						isCollectSuccess = true;

						//とりあえず壊しちゃう
						boss->parts[(int32_t)PartsNum::RightHand].collectPos = collectCol.start;
						boss->parts[(int32_t)PartsNum::RightHand].ChangeState<BossPartCollect>();
						//腕の吸収値も変数化したい
						waxCollectAmount += 1;
					}
				}
			}

			//本体吸収
			if (boss->GetStateStr() == "Collected") {
				if (RayToSphereCol(collectCol, boss->collider))
				{
					isCollectSuccess = true;

					//今のロウとの距離
					float len = (collectCol.start -
						boss->GetPos()).Length();

					//見たロウが範囲外ならスキップ
					if (waxCollectVertical >= len) {
						boss->collectPos = collectCol.start;
						boss->ChangeState<BossDeadState>();
						waxCollectAmount += 1;
					}
				}
			}

			if (isCollectSuccess) {
				obj.mModel = ModelManager::Get("collect");
				modelChange = true;
			}
		}
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

	hp -= damage;

	//パーティクル生成
	ParticleManager::GetInstance()->AddSimple(obj.mTransform.position, "player_hit");

	//ちょっとのけぞる
	//モーション遷移
	backwardTimer.maxTime_ = damageCoolTimer.maxTime_ / 2;
	backwardTimer.Start();
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
	return (RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_LEFT_SHOULDER) ||
		RInput::GetInstance()->GetLTriggerDown() ||
		RInput::GetInstance()->GetKeyDown(DIK_Q));
}
