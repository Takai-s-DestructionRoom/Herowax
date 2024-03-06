#include "Player.h"
#include "WaxManager.h"
#include "RInput.h"
#include "Camera.h"
#include "TimeManager.h"
#include "Util.h"
#include "Quaternion.h"
#include "RImGui.h"

Player::Player() :
	isJumping(false), jumpTimer(0.2f), jumpHeight(0.f), maxJumpHeight(5.f), jumpPower(2.f), jumpSpeed(0.f),
	isAttack(false), atkDist(1.f), atkRange({ 3.f,5.f }), atkSize(0.f), atkPower(1), atkCoolTimer(0.3f), atkTimer(0.5f)
{
	obj = ModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube", true));
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

	Attack();

	//地面に埋ってたら
	if (obj.mTransform.position.y - obj.mTransform.scale.y < 0.f)
	{
		//地面に触れるとこまで移動
		obj.mTransform.position.y = 0.f + obj.mTransform.scale.y;

		isGraund = true;
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
	ImGui::SetNextWindowSize({ 400, 250 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Player", NULL, window_flags);

	ImGui::Text("Lスティック移動、Aボタンジャンプ、Rで攻撃");
	ImGui::Text("WASD移動、スペースジャンプ、右クリで攻撃");

	if (ImGui::TreeNode("移動系"))
	{
		ImGui::Text("座標:%f,%f,%f", GetPos().x, GetPos().y, GetPos().z);
		ImGui::Text("動く方向:%f,%f,%f", moveVec.x, moveVec.y, moveVec.z);
		//ImGui::Text("正面ベクトル:%f,%f,%f", frontVec.x, frontVec.y, frontVec.z);
		ImGui::Text("ジャンプの高さ:%f", jumpHeight);
		ImGui::Text("ジャンプ速度:%f", jumpSpeed);
		ImGui::SliderFloat("移動速度:%f", &moveSpeed, 0.f, 5.f);
		ImGui::SliderFloat("重力:%f", &gravity, 0.f, 0.2f);
		ImGui::SliderFloat("ジャンプ力:%f", &jumpPower, 0.f, 5.f);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("攻撃系"))
	{
		ImGui::Text("攻撃中か:%d", isAttack);
		ImGui::SliderFloat("攻撃時間", &atkTimer.maxTime_, 0.f, 2.f);
		ImGui::SliderFloat("射出距離", &atkDist, 0.f, 5.f);
		ImGui::SliderFloat("攻撃範囲X", &atkRange.x, 0.f, 10.f);
		ImGui::SliderFloat("攻撃範囲Y", &atkRange.y, 0.f, 10.f);
		ImGui::SliderFloat("クールタイム", &atkCoolTimer.maxTime_, 0.f, 2.f);

		ImGui::TreePop();
	}

	if (ImGui::Button("Reset")) {
		SetPos({ 0, 0, 0 });
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
		moveVec *= stick.Length();								//傾き具合を大きさに反映

		moveVec *= moveSpeed;									//移動速度をかけ合わせたら完成
		obj.mTransform.position += moveVec;						//完成したものを座標に足し合わせる
	}

	//接地時にAボタン押すと
	if (isGraund && RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A))
	{
		isJumping = true;
		isGraund = false;
		jumpSpeed = jumpPower;	//速度に初速を代入
		jumpTimer.Start();
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
		moveVec *= Matrix4::RotationY(cameraRad + keyRad);		//カメラの角度から更にスティックの入力角度を足して
		moveVec.Normalize();									//方向だけの情報なので正規化して

		moveVec *= moveSpeed;									//移動速度をかけ合わせたら完成
		obj.mTransform.position += moveVec;						//完成したものを座標に足し合わせる
	}

	//接地時にAボタン押すと
	if (isGraund && RInput::GetInstance()->GetKeyDown(DIK_SPACE))
	{
		isJumping = true;
		isGraund = false;
		jumpSpeed = jumpPower;	//速度に初速を代入
		jumpTimer.Start();
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

void Player::Attack()
{
	if (isAttack == false)
	{
		if (RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER) ||
			RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_RIGHT_THUMB) ||
			RInput::GetInstance()->GetMouseClickDown(1))
		{
			isAttack = true;
			atkTimer.Start();

			//入力時の出現位置と方向を記録
			atkOriginPos = GetPos() + GetFrontVec();
			atkVec = GetFrontVec();

			//生成
			WaxManager::GetInstance()->Create(
			obj.mTransform,atkPower, atkVec,atkOriginPos,
				atkDist,atkRange,atkSize,atkTimer.maxTime_);
		}
	}

	//攻撃中は
	if (isAttack)
	{
		atkTimer.Update();
		atkCoolTimer.Update();
	}

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
}

Vector3 Player::GetFrontVec()
{
	//正面ベクトルを取得
	frontVec *= Quaternion::Euler(obj.mTransform.rotation);
	return frontVec;
}
