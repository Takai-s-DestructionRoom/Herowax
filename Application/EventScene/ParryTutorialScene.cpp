#include "ParryTutorialScene.h"
#include "Player.h"
#include "InstantDrawer.h"

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

	Camera::sNowCamera = &camera;

	eventTimer.Start();

	isActive = true;

	buttonUIPos = { RWindow::GetWidth() * 0.5f,RWindow::GetHeight() * 0.5f - 100.f };;
	buttonUIPos.y += 300.f;
}

void ParryTutorialScene::Update()
{
	eventTimer.Update();

	camera.mViewProjection.UpdateMatrix();

	if (Player::GetParryButtonDown()){
		Camera::sNowCamera = nullptr;
		isActive = false;
	}
}

void ParryTutorialScene::Draw()
{
	InstantDrawer::DrawGraph(
		buttonUIPos.x, buttonUIPos.y,
		0.12f, 0.12f, 0.f, TextureManager::Load("./Resources/UI/A_normal.png", "Abutton"));
}
