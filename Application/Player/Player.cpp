#include "Player.h"
#include "RInput.h"
#include "Camera.h"

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
		Vector3 cameraVec = Camera::sNowCamera->mViewProjection.mTarget - Camera::sNowCamera->mViewProjection.mEye;
		//float cameraRad = atan2f(cameraVec.x, cameraVec.z);
		//float stickRad = atan2f(stick.x, stick.y);

		moveVec = { stick.x, 0, stick.y };
		//moveVec *= Matrix4::RotationY(cameraRad + stickRad);
		//moveVec.Normalize();
		//moveVec *= stick.Length();

		moveVec *= moveSpeed;
		obj.mTransform.position += moveVec;
	}

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Player::Draw()
{
	obj.Draw();
}