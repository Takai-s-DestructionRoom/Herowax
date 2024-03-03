#include "ViewProjection.h"
#include <Float4.h>
#include <RWindow.h>

void ViewProjection::UpdateMatrix()
{
	mView = Matrix4::View(mEye, mTarget, mUpVec);
	if (mType == Type::Perspective) {
		mView = Matrix4::View(mEye, mTarget, mUpVec);
		mProjection = Matrix4::PerspectiveProjection(mFov, mAspect, mNearclip, mFarclip);
	}
	else if (mType == Type::Orthographic) {
		Vector3 _eye = mEye;
		_eye.x -= mOrthoSize.x / 2.0f;
		_eye.y -= mOrthoSize.y / 2.0f;
		Vector3 _target = mTarget;
		_target.x -= mOrthoSize.x / 2.0f;
		_target.y -= mOrthoSize.y / 2.0f;
		mView = Matrix4::View(_eye, _target, mUpVec);
		mProjection = Matrix4::OrthoGraphicProjection(0, mOrthoSize.x, 0, mOrthoSize.y, mNearclip, mFarclip);
	}
	mMatrix = mView * mProjection;
}

void ViewProjection::Transfer(ViewProjectionBuffer* pBuff)
{
	pBuff->matrix = mMatrix;
	pBuff->cameraPos = mEye;
}

Vector2 ViewProjection::WorldToScreen(Vector3 wPos)
{
	return WorldToScreen(wPos,
		0, 0,
		static_cast<float>(RWindow::GetWidth()),
		static_cast<float>(RWindow::GetHeight()),
		0, 1
	);
}

Vector2 ViewProjection::WorldToScreen(Vector3 wPos, float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
{
	Float4 screenPos = wPos;
	screenPos *= mMatrix;
	screenPos /= screenPos.w;
	screenPos *= Matrix4::Viewport(
		topLeftX, topLeftY,
		width, height,
		minDepth, maxDepth);
	return screenPos;
}
