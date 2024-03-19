#include "Player.h"
#include "WaxManager.h"
#include "RInput.h"
#include "Camera.h"
#include "TimeManager.h"
#include "ParticleManager.h"
#include "Util.h"
#include "Quaternion.h"
#include "RImGui.h"
#include "FireManager.h"
#include "Parameter.h"

Player::Player() :GameObject(),
moveSpeed(1.f), moveAccelAmount(0.05f), isGround(true), hp(0), maxHP(10.f),
isJumping(false), jumpTimer(0.2f), jumpHeight(0.f), maxJumpHeight(5.f), jumpPower(2.f), jumpSpeed(0.f),
isAttack(false), atkSpeed(1.f), atkRange({ 3.f,5.f }), atkSize(0.f), atkPower(1),
atkCoolTimer(0.3f), atkTimer(0.5f), atkHeight(1.f), solidTimer(5.f)
{
	obj = PaintableModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube", true));

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

	pabloRange = Parameter::GetParam(extract, "パブロ攻撃の広がり", 5.f);
	pabloSideRange = Parameter::GetParam(extract, "パブロ攻撃の横の広がり", 5.f);
	pabloSpeedMag = Parameter::GetParam(extract, "パブロ攻撃時の移動速度低下係数", 0.2f);
	shotDeadZone = Parameter::GetParam(extract, "ショットが出る基準", 0.5f);

	attackState = std::make_unique<PlayerNormal>();
}

void Player::Init()
{
	hp = maxHP;
}

void Player::Update()
{
	//パッド接続してたら
	if (RInput::GetInstance()->GetPadConnect())
	{
		MovePad();
	}
	else
	{
		MoveKey();
	}

	attackState->Update(this);

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

	Fire();

	//地面に埋ってたら
	if (obj.mTransform.position.y - obj.mTransform.scale.y < 0.f)
	{
		//地面に触れるとこまで移動
		obj.mTransform.position.y = 0.f + obj.mTransform.scale.y;

		if (isGround == false)
		{
			isGround = true;

			//エミッターの座標はプレイヤーの座標からY座標だけにスケール分ずらしたもの
			Vector3 emitterPos = obj.mTransform.position;
			emitterPos.y -= obj.mTransform.scale.y;

			ParticleManager::GetInstance()->AddRing(emitterPos,16, 0.3f, obj.mTuneMaterial.mColor, 0.7f, 1.2f, 0.3f, 0.6f,
				0.01f, 0.05f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f);
		}
		isJumping = false;
	}

	//HP0になったら死ぬ
	if (hp <= 0)
	{
		isAlive = false;
	}

	UpdateCollider();

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);

#pragma region ImGui
	ImGui::SetNextWindowSize({ 600, 250 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Player", NULL, window_flags);

	ImGui::Text("Lスティック移動、Aボタンジャンプ、Rで攻撃");
	ImGui::Text("WASD移動、スペースジャンプ、右クリで攻撃");
	ImGui::Text("obj.mTransform.rotation.x %f", obj.mTransform.rotation.x);
	ImGui::Text("obj.mTransform.rotation.y %f", obj.mTransform.rotation.y);
	ImGui::Text("obj.mTransform.rotation.z %f", obj.mTransform.rotation.z);

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

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("攻撃系"))
	{
		ImGui::Text("攻撃中か:%d", isAttack);
		ImGui::Checkbox("攻撃中でも次の攻撃を出せるか", &isMugenAttack);

		ImGui::SliderFloat("攻撃時間", &atkTimer.maxTime_, 0.f, 2.f);
		ImGui::SliderFloat("射出速度", &atkSpeed, 0.f, 2.f);
		ImGui::SliderFloat("射出高度", &atkHeight, 0.f, 3.f);
		ImGui::SliderFloat("攻撃範囲X", &atkRange.x, 0.f, 10.f);
		ImGui::SliderFloat("攻撃範囲Y", &atkRange.y, 0.f, 10.f);
		ImGui::SliderFloat("クールタイム", &atkCoolTimer.maxTime_, 0.f, 2.f);
		ImGui::SliderFloat("固まるまでの時間", &solidTimer.maxTime_, 0.f, 10.f);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("お試し実装:自分の方に来る"))
	{
		ImGui::Checkbox("攻撃した相手が自分を攻撃するか", &isTauntMode);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("お試し実装:パブロアタック"))
	{
		ImGui::Text("スティックの入力:%f", abs(RInput::GetInstance()->GetPadLStick().LengthSq()));
		ImGui::SliderFloat("ショットが出る基準", &shotDeadZone,0.0f,2.0f);
		ImGui::SliderFloat("広がり", &pabloRange,0.0f,10.f);
		ImGui::SliderFloat("横の広がり", &pabloSideRange,0.0f,10.f);
		ImGui::SliderFloat("パブロ攻撃時の移動速度低下係数", &pabloSpeedMag,0.0f,1.0f);

		ImGui::TreePop();
	}

	if (ImGui::Button("Reset")) {
		SetPos({ 0, 0, 0 });
	}
	if (ImGui::Button("セーブ")) {
		Parameter::Begin("Player");
		Parameter::Save("移動速度", moveSpeed);
		Parameter::Save("移動加速度", moveAccelAmount);
		Parameter::Save("重力", gravity);
		Parameter::Save("ジャンプ力", jumpPower);
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
		Parameter::Save("ショットが出る基準", shotDeadZone);
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
			2, 0.5f, obj.mTuneMaterial.mColor, 0.3f, 0.7f,
			{ -0.001f,0.01f,-0.001f }, { 0.001f,0.03f,0.001f },
			0.01f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f);
	}
	else
	{
		moveAccel -= moveAccelAmount;	//入力されてなければ徐々に減速
	}

	moveAccel = Util::Clamp(moveAccel, 0.f, 1.f);			//無限に増減しないよう抑える
	moveVec *= moveSpeed * moveAccel;						//移動速度をかけ合わせたら完成
	obj.mTransform.position += moveVec;						//完成したものを座標に足し合わせる

	//接地時にAボタン押すと
	if (isGround && RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A))
	{
		isJumping = true;
		isGround = false;
		jumpSpeed = jumpPower;	//速度に初速を代入
		jumpTimer.Start();

		//エミッターの座標はプレイヤーの座標からY座標だけにスケール分ずらしたもの
		Vector3 emitterPos = obj.mTransform.position;
		emitterPos.y -= obj.mTransform.scale.y;

		ParticleManager::GetInstance()->AddRing(emitterPos,
		20, 0.5f, obj.mTuneMaterial.mColor, 1.f, 2.5f, 0.5f, 0.8f,
			0.01f, 0.05f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f);
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

	//キー入力されてたら
	if (keyVec.LengthSq() > 0.f) {
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
	moveVec *= moveSpeed * moveAccel;						//移動速度をかけ合わせたら完成
	obj.mTransform.position += moveVec;						//完成したものを座標に足し合わせる

	//接地時にスペース押すと
	if (isGround && RInput::GetInstance()->GetKeyDown(DIK_SPACE))
	{
		isJumping = true;
		isGround = false;
		jumpSpeed = jumpPower;	//速度に初速を代入
		jumpTimer.Start();

		//エミッターの座標はプレイヤーの座標からY座標だけにスケール分ずらしたもの
		Vector3 emitterPos = obj.mTransform.position;
		emitterPos.y -= obj.mTransform.scale.y;

		ParticleManager::GetInstance()->AddRing(emitterPos, 16, 0.5f, obj.mTuneMaterial.mColor, 1.f, 2.5f, 0.5f, 0.8f,
			0.01f, 0.03f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f);
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
			2, 0.5f, obj.mTuneMaterial.mColor, 0.3f, 0.7f,
			{ -0.001f,0.01f,-0.001f }, { 0.001f,0.03f,0.001f },
			0.01f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f);
	}

	//「ジャンプの高さ」+「プレイヤーの大きさ」を反映
	obj.mTransform.position.y = jumpHeight + obj.mTransform.scale.y;
}

void Player::Attack()
{
	if (isAttack == false || isMugenAttack)
	{
		if ((RInput::GetInstance()->GetPadButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) ||
			RInput::GetInstance()->GetMouseClick(1)) &&
			!atkCoolTimer.GetRun())
		{
			isAttack = true;
			atkTimer.Start();

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

void Player::PabloAttack()
{
	//攻撃中なら次の攻撃が出せない
	if (atkCoolTimer.GetRun())return;
	atkCoolTimer.Start();

	Vector3 pabloVec = { 0,0,0 };

	pabloVec = GetFrontVec();
	pabloVec.y = atkHeight;

	pabloVec.Normalize();

	atkVec = pabloVec;

	Vector2 rotaVec = { pabloVec.x,pabloVec.z };
	rotaVec = rotaVec.Rotation(-Util::PI / 2);

	//pabloVecの横ベクトルを取る
	Vector3 sidePabloVec = { rotaVec.x,0,rotaVec.y };
	sidePabloVec.Normalize();
	
	//発射数の半分(切り捨て)はマイナス横ベクトル方向へずらす
	int32_t waxNum = 3;
	
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
		Transform spawnTrans = obj.mTransform;
		//横のランダムを決定
		spawnTrans.position.x += Util::GetRand(sideRandMin.x, sideRandMax.x);
		spawnTrans.position.z += Util::GetRand(sideRandMin.z, sideRandMax.z);
		
		//前に(幅 / 数)分進める(多少ランダムにしたい)
		spawnTrans.position += (pabloVec * pabloRange / (float)waxNum) * (float)i;

		WaxManager::GetInstance()->Create(
			spawnTrans, atkPower,
			atkVec, atkSpeed,
			atkRange, atkSize,
			atkTimer.maxTime_, solidTimer.maxTime_);
	}
}

void Player::Fire()
{
	//情報をfiremanagerへ送信
	FireManager::GetInstance()->SetTarget(&obj);
	FireManager::GetInstance()->SetThorwVec(GetFrontVec());

	//押し込んだら
	if (RInput::GetPadButtonDown(XINPUT_GAMEPAD_RIGHT_THUMB) ||
		RInput::GetInstance()->GetKeyDown(DIK_F))
	{
		//放物線上に炎を投げる
		FireManager::GetInstance()->Create();
	}
}

Vector3 Player::GetFrontVec()
{
	//正面ベクトルを取得
	frontVec = { 0,0,1 };
	frontVec *= Quaternion::Euler(obj.mTransform.rotation);
	return frontVec;
}
