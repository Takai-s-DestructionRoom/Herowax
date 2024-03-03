#include "SimpleSceneTransition.h"
#include "RWindow.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"

SimpleSceneTransition::SimpleSceneTransition() {
	TextureManager::Create({ 1, 1, 1, 1 }, 100, 100, "Shutter");
	shutter = Sprite("Shutter", {0.5f, 0.5f});
}

void SimpleSceneTransition::Update()
{
	if (mIsClosed) {
		if (shutterScale >= 1) {
			inProgress = false;
			shutterScale = 1;
			shutterAlpha = 1;
		}
		else {
			inProgress = true;
			timer += TimeManager::deltaTime;
			float t = min(1.0f, timer / 0.8f);

			shutterScale = (1.0f - powf(1 - t, 5));
			shutterAlpha = 1;
		}
	}
	else {
		if (shutterAlpha <= 0) {
			inProgress = false;
			shutterAlpha = 0;
		}
		else {
			inProgress = true;
			timer += TimeManager::deltaTime;
			float t = min(1.0f, timer / 0.8f);

			shutterAlpha = 1 - (1.0f - powf(1 - t, 5));
		}
	}

	float scale = 15.0f * shutterScale;
	shutter.mTransform.position = { RWindow::GetWidth() / 2.0f, RWindow::GetHeight() / 2.0f, 1145141919.0f };
	shutter.mTransform.rotation = { 0, 0, Util::AngleToRadian(180 * shutterScale)};
	shutter.mTransform.scale = { scale, scale, 1 };
	shutter.mMaterial.mColor = { 0, 0.5f, 0.7f, shutterAlpha };
	shutter.mTransform.UpdateMatrix();
	shutter.TransferBuffer();
}

void SimpleSceneTransition::Draw()
{
	if (shutterScale != 0) {
		shutter.Draw();
	}
}

void SimpleSceneTransition::Open()
{
	mIsClosed = false;
	timer = 0;
}

bool SimpleSceneTransition::IsOpened()
{
	return !mIsClosed && !inProgress && timer != 0;
}

void SimpleSceneTransition::Close()
{
	mIsClosed = true;
	timer = 0;
}

bool SimpleSceneTransition::IsClosed()
{
	return mIsClosed && !inProgress && timer != 0;
}

bool SimpleSceneTransition::InProgress()
{
	return inProgress;
}
