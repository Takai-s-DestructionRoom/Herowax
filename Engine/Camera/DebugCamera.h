/*
* @file DebugCamera.h
* @brief FPS的移動ができるデバッグ用カメラ
*/

#pragma once
#include "Camera.h"
#include "ViewProjection.h"
#include "Vector2.h"
#include "RWindow.h"

class DebugCamera : public Camera
{
public:
	bool mFreeFlag = false;
	float mMoveSpeed = 0.2f;
	float mSensitivity = 10.0f;
	Vector2 mAngle = { 90, 0 };

	DebugCamera(Vector3 pos, float moveSpeed = 0.2f, float sensitivity = 10.0f) : mMoveSpeed(moveSpeed), mSensitivity(sensitivity) {
		mViewProjection.mEye = pos;
		mViewProjection.mAspect = (float)RWindow::GetWidth() / RWindow::GetHeight();
	}

	void Update() override;
};

