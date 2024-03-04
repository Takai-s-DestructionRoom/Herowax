#include "Player.h"
#include "RInput.h"
#include "Camera.h"
#include "RImGui.h"

Player::Player() :
	moveSpeed(1.f), isJumping(false), jumpTimer(0.0f), maxJumpTimer(1.0f),
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
	if (obj.mTransform.position.y + obj.mTransform.scale.y > 0.f)
	{
		//地面に触れるとこまで移動
		obj.mTransform.position.y = 0.f + obj.mTransform.scale.y;
	}

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);

#pragma region ImGui
	ImGui::SetNextWindowSize({ 300, 150 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Player", NULL, window_flags);

	ImGui::Text("pos:%f,%f,%f", GetPos().x, GetPos().y, GetPos().z);
	ImGui::Text("moveVec:%f,%f,%f", moveVec.x, moveVec.y, moveVec.z);
	ImGui::SliderFloat("moveSpeed:%f", &moveSpeed, 0.f, 5.f);

	if (ImGui::Button("Reset")) {
		SetPos({ 0, 0, 0 });
	}

	ImGui::End();
#pragma endregion
}

void Player::Draw()
{
	obj.Draw();
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
	}

	if (isJumping) {
		/*velocity.y += 0.1f * TimeManager::deltaFrame;
		if (velocity.y > 0.2f)
		{
			velocity.y = 0.2f;
		}
		jumpTimer += TimeManager::deltaTime;*/

		//ジャンプ時間超えるか途中でAボタン離されたら
		if (jumpTimer >= maxJumpTimer || (!RInput::GetInstance()->GetPadButton(XINPUT_GAMEPAD_A)))
		{
			jumpTimer = 0;		//ジャンプタイマーリセット
			isJumping = false;	//ジャンプフラグもリセット
		}
	}
}
