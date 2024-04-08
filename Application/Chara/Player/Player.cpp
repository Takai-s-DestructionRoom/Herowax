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

Player::Player() :GameObject(),
moveSpeed(1.f), moveAccelAmount(0.05f), isGround(true), hp(0), maxHP(10.f),
isJumping(false), jumpTimer(0.2f), jumpHeight(0.f), maxJumpHeight(5.f), jumpPower(2.f), jumpSpeed(0.f),
isAttack(false), atkSpeed(1.f), atkRange({ 3.f,5.f }), atkSize(0.f), atkPower(1),
atkCoolTimer(0.3f), atkTimer(0.5f), atkHeight(1.f), solidTimer(5.f),
isFireStock(false), isWaxStock(true), isCollectFan(false), maxWaxStock(20)
{
	std::map<std::string, std::string> extract = Parameter::Extract("Player");
	moveSpeed = Parameter::GetParam(extract, "移動速度", 1.f);
	moveAccelAmount = Parameter::GetParam(extract, "移動加速度", 0.05f);
	gravity = Parameter::GetParam(extract, "重力", 0.098f);
	jumpPower = Parameter::GetParam(extract, "ジャンプ力", 2.0f);
	atkTimer.maxTime_ = Parameter::GetParam(extract, "攻撃時間", 0.5f);
	atkSpeed = Parameter::GetParam(extract, "射出速度", 1.f);
	atkHeight = Parameter::GetParam(extract, "射出高度", 1.f);
	atkRange.x = Parameter::GetParam(extract, "攻撃範囲X", 3.f);
	atkRange.y = Parameter::GetParam(extract, "攻撃範囲Y", 5.f);
	atkCoolTimer.maxTime_ = Parameter::GetParam(extract, "クールタイム", 0.3f);
	solidTimer.maxTime_ = Parameter::GetParam(extract, "固まるまでの時間", 5.f);
	atkPower = (int32_t)Parameter::GetParam(extract, "敵に与えるダメージ", 10.0f);

	pabloRange = Parameter::GetParam(extract, "パブロ攻撃の広がり", 5.f);
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
	attackHitCollider.r = Parameter::GetParam(extract,"敵がこの範囲に入ると攻撃状態へ遷移する大きさ",1.0f);

	collectRangeModel = ModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube", true));
	waxCollectRange = Parameter::GetParam(extract, "ロウ回収範囲", 5.f);
	collectRangeModel.mTuneMaterial.mColor.a = Parameter::GetParam(extract, "範囲objの透明度", 0.5f);
	collectRangeModel.mTuneMaterial.mAmbient = Vector3::ONE * 100.f;
	collectRangeModel.mTuneMaterial.mDiffuse = Vector3::ZERO;
	collectRangeModel.mTuneMaterial.mSpecular = Vector3::ZERO;

	collectRangeModelCircle = ModelObj(Model::Load("./Resources/Model/wax/wax.obj", "Wax", true));
	waxCollectDist = Parameter::GetParam(extract, "ロウ回収半径", 5.f);
	waxCollectAngle = Parameter::GetParam(extract, "ロウ回収角度", 90.f);
	collectRangeModelCircle.mTuneMaterial.mColor.a = Parameter::GetParam(extract, "範囲(扇)objの透明度", 0.5f);

	collectRangeModelRayLeft = ModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube", true));
	collectRangeModelRayRight = ModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube", true));

	attackState = std::make_unique<PlayerNormal>();

	attackDrawerObj = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));
}

void Player::Init()
{
	obj = PaintableModelObj(Model::Load("./Resources/Model/player/player_bird.obj", "player_bird", true));

	hp = maxHP;
	//fireUnit.Init();

	waxStock = maxWaxStock;

	//初期値適用
	obj.mTransform.position = initPos;

	Vector3 rot;
	rot.x = Util::AngleToRadian(initRot.x);
	rot.y = Util::AngleToRadian(initRot.y);
	rot.z = Util::AngleToRadian(initRot.z);
	obj.mTransform.rotation = rot;
	obj.mTransform.UpdateMatrix();
}

void Player::Update()
{
	//無敵時間更新
	mutekiTimer.Update();
	//ダメージ時点滅
	DamageBlink();

	//パッド接続してたら
	if (RInput::GetInstance()->GetPadConnect())
	{
		MovePad();
	}
	else
	{
		MoveKey();
	}

	Rotation();

	attackState->Update(this);
	//前のステートと異なれば
	if (changingState) {
		//ステートを変化させる
		std::swap(attackState, nextState);
		changingState = false;
		nextState = nullptr;
	}

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
		isAttack = false;
		atkCoolTimer.Reset();
	}

	//地面に埋ってたら
	if (obj.mTransform.position.y - obj.mTransform.scale.y < 0.f)
	{
		//地面に触れるとこまで移動
		obj.mTransform.position.y = 0.f + obj.mTransform.scale.y;

		if (isGround == false)
		{
			isGround = true;

			//エミッターの座標はプレイヤーの座標からY座標だけにスケール分ずらしたもの
			Vector3 emitterPos = GetCenterPos();

			ParticleManager::GetInstance()->AddRing(
				emitterPos, 16, 0.3f, obj.mTuneMaterial.mColor, "",
				0.7f, 1.2f, 0.3f, 0.6f, 0.01f, 0.05f,
				-Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f);
		}
		isJumping = false;
	}

	//HP0になったら死ぬ
	if (hp <= 0)
	{
		isAlive = false;
	}

	UpdateCollider();
	UpdateAttackCollider();
	
	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);

	ui.Update(this);

	WaxCollect();

	if (isWaxStock == false)
	{
		waxStock = maxWaxStock;
	}
	//ストックがおかしな値にならないように
	waxStock = Util::Clamp(waxStock, 0, maxWaxStock);

	/*fireUnit.SetTransform(obj.mTransform);
	fireUnit.SetIsFireStock(isFireStock);
	fireUnit.Update();*/

#pragma region ImGui
	ImGui::SetNextWindowSize({ 600, 250 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Player", NULL, window_flags);

	ImGui::Text("Lスティック移動、Aボタンジャンプ、Rで攻撃,Lでロウ回収");
	ImGui::Text("WASD移動、スペースジャンプ、右クリで攻撃,Pでパブロ攻撃,Qでロウ回収");

	ImGui::ColorEdit4("プレイヤーの色", &obj.mTuneMaterial.mColor.r);

	if (ImGui::TreeNode("移動系"))
	{
		ImGui::Text("座標:%f,%f,%f", GetPos().x, GetPos().y, GetPos().z);
		ImGui::Text("動く方向:%f,%f,%f", moveVec.x, moveVec.y, moveVec.z);
		//ImGui::Text("正面ベクトル:%f,%f,%f", frontVec.x, frontVec.y, frontVec.z);
		ImGui::Text("ジャンプの高さ:%f", jumpHeight);
		ImGui::Text("ジャンプ速度:%f", jumpSpeed);
		ImGui::Text("加速度:%f", moveAccel);
		ImGui::SliderFloat("移動速度:%f", &moveSpeed, 0.f, 5.f);
		ImGui::SliderFloat("移動加速度:%f", &moveAccelAmount, 0.f, 0.1f);
		ImGui::SliderFloat("重力:%f", &gravity, 0.f, 0.2f);
		ImGui::SliderFloat("ジャンプ力:%f", &jumpPower, 0.f, 5.f);

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
	if (ImGui::TreeNode("攻撃系"))
	{
		ImGui::Text("攻撃中か:%d", isAttack);
		ImGui::Checkbox("攻撃中でも次の攻撃を出せるか", &isMugenAttack);
		ImGui::Checkbox("ロウをストック性にするか", &isWaxStock);
		ImGui::Checkbox("回収範囲を扇型にするか", &isCollectFan);
		ImGui::Checkbox("炎をストック性にするか", &isFireStock);

		ImGui::InputInt("敵に与えるダメージ", &atkPower, 1);
		ImGui::SliderFloat("攻撃時間", &atkTimer.maxTime_, 0.f, 2.f);
		ImGui::SliderFloat("射出速度", &atkSpeed, 0.f, 2.f);
		ImGui::SliderFloat("射出高度", &atkHeight, 0.f, 3.f);
		ImGui::SliderFloat("攻撃範囲X", &atkRange.x, 0.f, 10.f);
		ImGui::SliderFloat("攻撃範囲Y", &atkRange.y, 0.f, 10.f);
		ImGui::SliderFloat("クールタイム", &atkCoolTimer.maxTime_, 0.f, 2.f);
		ImGui::SliderFloat("固まるまでの時間", &solidTimer.maxTime_, 0.f, 10.f);
		ImGui::InputInt("ロウの最大ストック数", &maxWaxStock, 1, 100);
		ImGui::Text("ロウのストック数:%d", waxStock);
		//ImGui::Text("炎のストック数:%d", fireUnit.fireStock);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("お自分の方に来る"))
	{
		ImGui::Checkbox("攻撃した相手が自分を攻撃するか", &isTauntMode);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("パブロアタック"))
	{
		ImGui::Text("スティックの入力:%f", abs(RInput::GetInstance()->GetPadLStick().LengthSq()));
		ImGui::SliderFloat("ショットが出る基準", &shotDeadZone, 0.0f, 2.0f);
		ImGui::InputInt("一度に出るロウの数", &waxNum, 1);
		ImGui::SliderFloat("奥方向への広がり", &pabloRange, 0.0f, 10.f);
		ImGui::SliderFloat("横の広がり", &pabloSideRange, 0.0f, 10.f);
		ImGui::SliderFloat("パブロ攻撃時の移動速度低下係数", &pabloSpeedMag, 0.0f, 1.0f);
		ImGui::SliderFloat("パブロ攻撃を移動しながら撃った時の係数", &pabloShotSpeedMag, 1.0f, 5.f);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("ロウ回収系"))
	{
		ImGui::SliderFloat("ロウ回収範囲", &waxCollectRange, 0.f, 100.f);
		ImGui::SliderFloat("範囲objの透明度", &collectRangeModel.mTuneMaterial.mColor.a, 0.f, 1.f);
		ImGui::SliderFloat("ロウ回収半径", &waxCollectDist, 0.f, 100.f);
		ImGui::SliderFloat("ロウ回収角度", &waxCollectAngle, 1.f, 180.f);
		ImGui::SliderFloat("範囲(扇)objの透明度", &collectRangeModelCircle.mTuneMaterial.mColor.a, 0.f, 1.f);
		ImGui::Text("回収できる状態か:%d", WaxManager::GetInstance()->isCollected);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("当たり判定"))
	{
		ImGui::Checkbox("当たり判定描画切り替え", &isDrawCollider);
		ImGui::InputFloat("敵がこの範囲に入ると攻撃状態へ遷移する大きさ", &attackHitCollider.r, 1.0f);
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
		Parameter::Save("射出速度", atkSpeed);
		Parameter::Save("射出高度", atkHeight);
		Parameter::Save("攻撃範囲X", atkRange.x);
		Parameter::Save("攻撃範囲Y", atkRange.y);
		Parameter::Save("クールタイム", atkCoolTimer.maxTime_);
		Parameter::Save("固まるまでの時間", solidTimer.maxTime_);
		Parameter::Save("パブロ攻撃の広がり", pabloRange);
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
		Parameter::End();
	}

	ImGui::End();
#pragma endregion
}

void Player::Draw()
{
	if (isAlive)
	{
		obj.Draw();
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
			emitterPos, obj.mTransform.scale * 0.5f,
			2, 0.5f, obj.mTuneMaterial.mColor, "", 0.3f, 0.7f,
			{ -0.001f,0.01f,-0.001f }, { 0.001f,0.03f,0.001f },
			0.01f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.1f, 0.f, false, false);
	}
	else
	{
		moveAccel -= moveAccelAmount;	//入力されてなければ徐々に減速
	}

	moveAccel = Util::Clamp(moveAccel, 0.f, 1.f);			//無限に増減しないよう抑える
	moveVec *=
		moveSpeed * moveAccel *
		WaxManager::GetInstance()->isCollected;				//移動速度をかけ合わせたら完成(回収中は動けない)
	obj.mTransform.position += moveVec;						//完成したものを座標に足し合わせる

	//接地してて回収中じゃない時にAボタン押すと
	if (isGround && RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A) &&
		WaxManager::GetInstance()->isCollected)
	{
		isJumping = true;
		isGround = false;
		jumpSpeed = jumpPower;	//速度に初速を代入
		jumpTimer.Start();

		//エミッターの座標はプレイヤーの座標からY座標だけにスケール分ずらしたもの
		Vector3 emitterPos = GetCenterPos();

		ParticleManager::GetInstance()->AddRing(
			emitterPos, 20, 0.5f, obj.mTuneMaterial.mColor, "",
			1.f, 2.5f, 0.5f, 0.8f, 0.01f, 0.05f,
			-Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f);
	}

	//ジャンプ中は
	if (isJumping) {
		jumpTimer.Update();
		//速度に対して重力をかけ続けて減速
		jumpSpeed -= gravity;
		//高さに対して速度を足し続ける
		jumpHeight += jumpSpeed;

		//途中でAボタン離されたら
		//if ((!RInput::GetInstance()->GetPadButton(XINPUT_GAMEPAD_A)))
		//{
		//	//ジャンプ系統で使ってたものリセット
		//	jumpTimer.Reset();
		//	if (jumpSpeed > 0.f)
		//	{
		//		jumpSpeed = 0.f;
		//	}
		//}
	}
	else
	{
		//ジャンプしてないときはジャンプの高さを0に
		jumpHeight = 0.f;
	}

	//「ジャンプの高さ」+「プレイヤーの大きさ」を反映
	obj.mTransform.position.y = jumpHeight + obj.mTransform.scale.y;
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
	moveVec *=
		moveSpeed * moveAccel *
		WaxManager::GetInstance()->isCollected;				//移動速度をかけ合わせたら完成(回収中は動けない)
	obj.mTransform.position += moveVec;						//完成したものを座標に足し合わせる

	//接地時で回収中じゃない時にスペース押すと
	if (isGround && RInput::GetInstance()->GetKeyDown(DIK_SPACE) &&
		WaxManager::GetInstance()->isCollected)
	{
		isJumping = true;
		isGround = false;
		jumpSpeed = jumpPower;	//速度に初速を代入
		jumpTimer.Start();

		//エミッターの座標はプレイヤーの座標からY座標だけにスケール分ずらしたもの
		Vector3 emitterPos = GetCenterPos();

		ParticleManager::GetInstance()->AddRing(
			emitterPos, 16, 0.5f, obj.mTuneMaterial.mColor, "",
			1.f, 2.5f, 0.5f, 0.8f, 0.01f, 0.03f,
			-Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f);
	}

	//ジャンプ中は
	if (isJumping) {
		jumpTimer.Update();
		//速度に対して重力をかけ続けて減速
		jumpSpeed -= gravity;
		//高さに対して速度を足し続ける
		jumpHeight += jumpSpeed;

		//途中でAボタン離されたら
		//if ((!RInput::GetInstance()->GetPadButton(XINPUT_GAMEPAD_A)))
		//{
		//	//ジャンプ系統で使ってたものリセット
		//	jumpTimer.Reset();
		//	if (jumpSpeed > 0.f)
		//	{
		//		jumpSpeed = 0.f;
		//	}
		//}
	}
	else
	{
		//ジャンプしてないときはジャンプの高さを0に
		jumpHeight = 0.f;
	}

	if (keyVec.LengthSq() > 0.f || isJumping) {
		//エミッターの座標はプレイヤーの座標から移動方向の逆側にスケール分ずらしたもの
		Vector3 emitterPos = obj.mTransform.position;
		Vector2 mVelo = { -moveVec.x,-moveVec.z };	//moveVecを正規化すると実際の移動に支障が出るので一時変数に格納
		mVelo.Normalize();
		emitterPos.x += mVelo.x * obj.mTransform.scale.x;
		emitterPos.z += mVelo.y * obj.mTransform.scale.z;

		ParticleManager::GetInstance()->AddSimple(
			emitterPos, obj.mTransform.scale * 0.5f,
			2, 0.5f, obj.mTuneMaterial.mColor, TextureManager::Load("./Resources/white2x2.png"), 0.3f, 0.7f,
			{ -0.001f,0.01f,-0.001f }, { 0.001f,0.03f,0.001f },
			0.01f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f, 0.f, false, false);
	}

	//「ジャンプの高さ」+「プレイヤーの大きさ」を反映
	obj.mTransform.position.y = jumpHeight + obj.mTransform.scale.y;
}

void Player::Rotation()
{
	Vector2 RStick = RInput::GetInstance()->GetRStick(false, true);

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
		obj.mTransform.rotation = aLookat.ToEuler();
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
		obj.mTransform.rotation = aLookat.ToEuler();
	}
}

void Player::Attack()
{
	if (waxStock > 0)
	{
		if (isAttack == false || isMugenAttack)
		{
			if ((RInput::GetInstance()->GetPadButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) ||
				RInput::GetInstance()->GetMouseClick(1)) &&
				!atkCoolTimer.GetRun())
			{
				//ストック減らす
				waxStock--;

				isAttack = true;
				atkTimer.Start();

				//ホントは塗った面積に応じて溜めたい
				//fireUnit.FireGaugeCharge(1.f);

				//入力時の出現位置と方向を記録
				atkVec = GetFrontVec();
				atkVec.y = atkHeight;

				//生成
				WaxManager::GetInstance()->Create(
					obj.mTransform, atkPower, atkVec, atkSpeed,
					atkRange, atkSize, atkTimer.maxTime_, solidTimer.maxTime_);

				//すぐ攻撃のクールタイム始まるように
				if (isMugenAttack)
				{
					atkCoolTimer.Start();
					atkTimer.Reset();
				}
			}
		}
	}
}

void Player::PabloAttack()
{
	//攻撃中かストックないなら次の攻撃が出せない
	if (atkCoolTimer.GetRun() || waxStock <= 0)return;
	atkCoolTimer.Start();

	Vector3 pabloVec = { 0,0,0 };
	//入力があるならそっちへ
	pabloVec = GetFrontVec();
	pabloVec.y = atkHeight;

	//ホントは塗った面積に応じて溜めたい
	//fireUnit.FireGaugeCharge(1.f);

	atkVec = pabloVec;

	Vector2 rotaVec = { pabloVec.x,pabloVec.z };
	rotaVec = rotaVec.Rotation(-Util::PI / 2);

	//pabloVecの横ベクトルを取る
	Vector3 sidePabloVec = { rotaVec.x,0,rotaVec.y };
	sidePabloVec.Normalize();

	//発射数の半分(切り捨て)はマイナス横ベクトル方向へずらす

	//imguiでいじれるようにするのと、前方向へのランダムを作る

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

		//前に(幅 / 数)分進める(多少ランダムにしたい)
		spawnTrans.position += (pabloVec * pabloRange / (float)waxNum) * (float)i;

		Vector2 stick = RInput::GetInstance()->GetPadLStick();
		//stick.LengthSq()は大体0~1.0(斜めで1.2)くらいが返ってくるので、
		//そのまま係数を掛ける
		float hoge = stick.LengthSq() * pabloShotSpeedMag;
		//最低でも1になってほしいのでclamp
		hoge = Util::Clamp(hoge, 1.f, 100.f);
		float atkVal = atkSpeed * hoge;

		WaxManager::GetInstance()->Create(
			spawnTrans, atkPower,
			atkVec, atkVal,
			atkRange, atkSize,
			atkTimer.maxTime_, solidTimer.maxTime_);
	}
}

void Player::WaxCollect()
{
	//扇の範囲表す用レイ
	collectRangeModelRayLeft.mTransform = obj.mTransform;
	collectRangeModelRayRight.mTransform = obj.mTransform;
	collectRangeModelRayLeft.mTransform.scale = { 0.1f,0.1f,waxCollectDist*2.f };
	collectRangeModelRayRight.mTransform.scale = { 0.1f,0.1f,waxCollectDist*2.f };
	collectRangeModelRayLeft.mTransform.rotation.y += Util::AngleToRadian(-waxCollectAngle * 0.5f);
	collectRangeModelRayRight.mTransform.rotation.y += Util::AngleToRadian(waxCollectAngle * 0.5f);

	/*collectRangeModelRayLeft.mTransform.position += GetFrontVec() * waxCollectDist * 0.5f;
	collectRangeModelRayRight.mTransform.position += GetFrontVec() * waxCollectDist * 0.5f;*/

	collectRangeModelRayLeft.mTransform.UpdateMatrix();
	collectRangeModelRayLeft.TransferBuffer(Camera::sNowCamera->mViewProjection);
	collectRangeModelRayRight.mTransform.UpdateMatrix();
	collectRangeModelRayRight.TransferBuffer(Camera::sNowCamera->mViewProjection);

	//トランスフォームはプレイヤー基準に
	collectRangeModel.mTransform = obj.mTransform;
	collectRangeModel.mTransform.scale = { waxCollectRange,0.1f,1000.f };

	collectRangeModelCircle.mTransform = obj.mTransform;
	collectRangeModelCircle.mTransform.scale = { waxCollectDist,0.1f,waxCollectDist };

	//大きさ分前に置く
	collectRangeModel.mTransform.position += GetFrontVec() * 1000.f * 0.5f;

	//更新
	collectRangeModel.mTransform.UpdateMatrix();
	collectRangeModel.TransferBuffer(Camera::sNowCamera->mViewProjection);

	collectRangeModelCircle.mTransform.UpdateMatrix();
	collectRangeModelCircle.TransferBuffer(Camera::sNowCamera->mViewProjection);

	//当たり判定で使うレイの設定
	collectCol.dir = GetFrontVec();
	collectCol.start = GetFootPos();
	collectCol.radius = waxCollectRange * 0.5f;

	//当たり判定で使う球の設定
	collectColFan.pos = GetFootPos();
	collectColFan.r = waxCollectDist;

	//回収したロウに応じてストック増やす
	if (isCollectFan)
	{

	}
	else
	{
		if (isWaxStock && WaxManager::GetInstance()->isCollected)
		{
			if (waxCollectAmount > 0)
			{
				waxStock += waxCollectAmount;
				waxCollectAmount = 0;
			}

			//最大量を超えて回収してたら最大量を増やす
			if (waxStock > maxWaxStock)
			{
				maxWaxStock = waxStock;
			}
		}
	}

	//回収ボタンポチーw
	if ((RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_LEFT_SHOULDER) ||
		RInput::GetInstance()->GetLTriggerDown() ||
		RInput::GetInstance()->GetKeyDown(DIK_Q)))
	{
		//ロウがストック性かつ地面についてて回収できる状態なら
		if (isWaxStock && isGround && WaxManager::GetInstance()->isCollected)
		{
			if (isCollectFan)
			{
				//ロウ回収
				WaxManager::GetInstance()->CollectFan(collectColFan,GetFrontVec(), waxCollectAngle);
			}
			else
			{
				//ロウ回収
				waxCollectAmount = WaxManager::GetInstance()->Collect(collectCol);
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

void Player::DealDamage(uint32_t damage)
{
	//無敵時間中ならダメージが与えられない
	if (mutekiTimer.GetRun())return;
	
	mutekiTimer.Start();
	blinkTimer.Start();

	hp -= damage;
}

void Player::DamageBlink()
{
	blinkTimer.RoopReverse();

	blinkTimer.maxTime_ = mutekiTimer.maxTime_ / (blinkNum * 2);

	//ダメージ処理中なら
	if (mutekiTimer.GetRun()) {
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