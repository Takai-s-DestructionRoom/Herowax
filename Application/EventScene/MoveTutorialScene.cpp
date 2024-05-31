#include "MoveTutorialScene.h"
#include "Player.h"
#include "InstantDrawer.h"

MoveTutorialScene::MoveTutorialScene()
{
	moveCountTimer = 2.f;
	cameraMoveCountTimer = 2.f;
}

MoveTutorialScene::~MoveTutorialScene()
{
}

void MoveTutorialScene::Init(const Vector3 target)
{
	camera.mViewProjection.mEye = Camera::sNowCamera->mViewProjection.mEye;
	camera.mViewProjection.mTarget = target;
	camera.mViewProjection.UpdateMatrix();

	Camera::sNowCamera = &camera;

	eventTimer.Start();

	isActive = true;

	tutorialUIPos = { RWindow::GetWidth() * 0.5f,RWindow::GetHeight() * 0.5f - 100.f };;
	tutorialUIPos.y += 300.f;

	moveCountTimer.Start();
	cameraMoveCountTimer.Start();
}

void MoveTutorialScene::Update()
{
	eventTimer.Update();

	camera.mViewProjection.UpdateMatrix();

	////動いてる間
	//if ()
	//{
	//	moveCountTimer.Update();
	//}
	////カメラ動かしてる間
	//if ()
	//{
	//	cameraMoveCountTimer.Update();
	//}

	//if (moveCountTimer.GetEnd() && cameraMoveCountTimer.GetEnd()) {
	//	Camera::sNowCamera = nullptr;
	//	isActive = false;
	//}
}

void MoveTutorialScene::Draw()
{
	InstantDrawer::DrawGraph(
		tutorialUIPos.x, tutorialUIPos.y,
		0.12f, 0.12f, 0.f, TextureManager::Load("./Resources/UI/A_normal.png", "Abutton"));
}
