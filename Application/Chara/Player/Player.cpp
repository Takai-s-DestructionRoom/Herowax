#include "Player.h"
#include "RInput.h"
#include "Camera.h"
#include "TimeManager.h"
#include "Util.h"
#include "RImGui.h"

Player::Player() :
	moveSpeed(1.f), isJumping(false), jumpTimer(0.2f),
	jumpHeight(0.f), maxJumpHeight(5.f), jumpPower(2.f),jumpSpeed(0.f),
	isGraund(true), hp(0), maxHP(10), isAlive(true)
{
	obj = ModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube", true));
}

void Player::Init()
{
	hp = maxHP;
}

void Player::Update()
{
	Move();

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

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
	
#pragma region ImGui
	ImGui::SetNextWindowSize({ 300, 250 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Player", NULL, window_flags);

	ImGui::Text("Lスティック移動、Aボタンジャンプ");
	ImGui::Text("pos:%f,%f,%f", GetPos().x, GetPos().y, GetPos().z);
	ImGui::Text("moveVec:%f,%f,%f", moveVec.x, moveVec.y, moveVec.z);
	ImGui::Text("jumpHeight:%f", jumpHeight);
	ImGui::Text("jumpSpeed:%f", jumpSpeed);
	ImGui::SliderFloat("moveSpeed:%f", &moveSpeed, 0.f, 5.f);
	ImGui::SliderFloat("gravity:%f", &gravity, 0.f, 0.2f);
	ImGui::SliderFloat("jumpPower:%f", &jumpPower, 0.f, 5.f);

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

void Player::Move()
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
