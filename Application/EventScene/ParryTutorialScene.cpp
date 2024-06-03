#include "ParryTutorialScene.h"
#include "Player.h"
#include "InstantDrawer.h"
#include "RAudio.h"

ParryTutorialScene::ParryTutorialScene()
{
}

ParryTutorialScene::~ParryTutorialScene()
{
}

void ParryTutorialScene::Init(const Vector3 target)
{
	camera.mViewProjection.mEye = Camera::sNowCamera->mViewProjection.mEye;
	camera.mViewProjection.mTarget = target;
	camera.mViewProjection.UpdateMatrix();
	RAudio::Load("Resources/Sounds/SE/A_select.wav", "Select");
	Camera::sNowCamera = &camera;

	eventTimer.Start();

	isActive = true;

	buttonUIPos = { RWindow::GetWidth() * 0.5f,RWindow::GetHeight() * 0.5f - 100.f };
	buttonUIPos.y += 300.f;

	textUIPos = { RWindow::GetWidth() * 0.5f,RWindow::GetHeight() * 0.5f - 100.f };
	textUIPos.y -= 100.f;
}

void ParryTutorialScene::Update()
{
	eventTimer.Update();

	camera.mViewProjection.UpdateMatrix();

	if (Player::GetParryButtonDown()){
		Camera::sNowCamera = nullptr;
		RAudio::Play("Select", 1.4f);
		isActive = false;
	}
}

void ParryTutorialScene::Draw()
{
	flashingTimer.Roop();
	//ボタンを表示
	if (flashingTimer.GetTimeRate() > 0.8f)
	{
		InstantDrawer::DrawGraph(
			buttonUIPos.x, buttonUIPos.y,
			0.5f, 0.5f, 0.f, TextureManager::Load("./Resources/UI/X_push.png", "XbuttonPush"));
	}
	else
	{
		InstantDrawer::DrawGraph(
			buttonUIPos.x, buttonUIPos.y,
			0.5f, 0.5f, 0.f, TextureManager::Load("./Resources/UI/X_normal2.png", "Xbutton"));
	}

	InstantDrawer::DrawGraph(
		textUIPos.x, textUIPos.y,
		0.5f, 0.5f, 0.f, TextureManager::Load("./Resources/UI/tutorial_guard.png", "tutorial_guard"));
}
