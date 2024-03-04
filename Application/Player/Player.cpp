#include "Player.h"
#include "RInput.h"
#include "Camera.h"
#include "RImGui.h"

Player::Player():
	moveSpeed(1.f),isJumping(false),jumpTimer(0.0f),maxJumpTimer(1.0f),
	isGraund(true), hp(0),maxHP(10),isAlive(true)
{
	obj = ModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube", true));
}

void Player::Init()
{
	hp = maxHP;
}

void Player::Update()
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

		moveVec = { 0, 0, 1 };
		moveVec *= Matrix4::RotationY(cameraRad + stickRad);
		moveVec.Normalize();
		moveVec *= stick.Length();

		moveVec *= moveSpeed;
		obj.mTransform.position += moveVec;
	}

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);

	// ImGui //
	ImGui::SetNextWindowSize({ 400, 150 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Player", NULL, window_flags);

	ImGui::Text("pos:%f,%f,%f", GetPos().x, GetPos().y, GetPos().z);
	ImGui::Text("moveVec:%f,%f,%f", moveVec.x, moveVec.y, moveVec.z);

	if (ImGui::Button("Reset")) {
		SetPos({ 0, 0, 0 });
	}

	ImGui::End();
}

void Player::Draw()
{
	obj.Draw();
}