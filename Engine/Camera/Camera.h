/*
* @file Camera.h
* @brief カメラ。
*/

#pragma once
#include <memory>
#include "ViewProjection.h"
#include "RWindow.h"
#include "SRConstBuffer.h"

class Camera
{
public:
	ViewProjection mViewProjection;
	SRConstBuffer<ViewProjectionBuffer> mBuff;

	Camera() {
		mViewProjection.mAspect = (float)RWindow::GetWidth() / RWindow::GetHeight();
		mViewProjection.UpdateMatrix();
		mViewProjection.Transfer(mBuff.Get());
	}
	virtual ~Camera() {}

	virtual void Update() {}

	static Camera* sNowCamera;
};

