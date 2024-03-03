#include "DebugCamera.h"
#include "RInput.h"

void DebugCamera::Update()
{
	if (RInput::GetKeyDown(DIK_LCONTROL)) {
		mFreeFlag = !mFreeFlag;
		return;
	}

	if (!mFreeFlag) {
		if (RInput::GetKey(DIK_W)) {
			mViewProjection.mEye.x += -mMoveSpeed * cosf(Util::PI / 180 * mAngle.x);
			mViewProjection.mEye.z += mMoveSpeed * sinf(Util::PI / 180 * mAngle.x);
		}
		if (RInput::GetKey(DIK_S)) {
			mViewProjection.mEye.x += -mMoveSpeed * cosf(Util::PI / 180 * (mAngle.x + 180));
			mViewProjection.mEye.z += mMoveSpeed * sinf(Util::PI / 180 * (mAngle.x + 180));
		}
		if (RInput::GetKey(DIK_A)) {
			mViewProjection.mEye.x += -mMoveSpeed * cosf(Util::PI / 180 * (mAngle.x - 90));
			mViewProjection.mEye.z += mMoveSpeed * sinf(Util::PI / 180 * (mAngle.x - 90));
		}
		if (RInput::GetKey(DIK_D)) {
			mViewProjection.mEye.x += -mMoveSpeed * cosf(Util::PI / 180 * (mAngle.x + 90));
			mViewProjection.mEye.z += mMoveSpeed * sinf(Util::PI / 180 * (mAngle.x + 90));
		}
		if (RInput::GetKey(DIK_SPACE)) {
			mViewProjection.mEye.y += mMoveSpeed;
		}
		if (RInput::GetKey(DIK_LSHIFT)) {
			mViewProjection.mEye.y -= mMoveSpeed;
		}

		mAngle.x += RInput::GetMouseMove().x / mSensitivity;
		mAngle.y -= RInput::GetMouseMove().y / mSensitivity;
	}

	if (mAngle.x >= 360) {
		mAngle.x -= 360;
	}
	if (mAngle.x < 0) {
		mAngle.x += 360;
	}

	if (mAngle.y >= 90) {
		mAngle.y = 89.9f;
	}
	if (mAngle.y <= -90) {
		mAngle.y = -89.9f;
	}

	mViewProjection.mTarget.x = mViewProjection.mEye.x + -100 * cosf(Util::PI / 180 * mAngle.x) * cosf(Util::PI / 180 * mAngle.y);
	mViewProjection.mTarget.y = mViewProjection.mEye.y + 100 * sinf(Util::PI / 180 * mAngle.y);
	mViewProjection.mTarget.z = mViewProjection.mEye.z + 100 * sinf(Util::PI / 180 * mAngle.x) * cosf(Util::PI / 180 * mAngle.y);

	mViewProjection.UpdateMatrix();
	mBuff->cameraPos = mViewProjection.mEye;
	mBuff->matrix = mViewProjection.mMatrix;
}
