#include "MoveTutorialScene.h"
#include "Player.h"
#include "InstantDrawer.h"
#include "GameCamera.h"

MoveTutorialScene::MoveTutorialScene()
{

}

MoveTutorialScene::~MoveTutorialScene()
{
}

void MoveTutorialScene::Init(const Vector3 target)
{
	camera.mViewProjection.mEye = Camera::sNowCamera->mViewProjection.mEye;
	camera.mViewProjection.mTarget = target;
	camera.mViewProjection.UpdateMatrix();

	Camera::sNowCamera = &GameCamera::GetInstance()->camera;

	eventTimer.Start();

	isActive = true;

	tutorialUIPos = { RWindow::GetWidth() * 0.5f,RWindow::GetHeight() * 0.5f - 100.f };
	tutorialUIPos.y += 300.f;

	moveCountGauge.Init("moveCountGauge");
	cameraCountGauge.Init("cameraCountGauge");

	moveCountGauge.baseRadian = 360.f;
	cameraCountGauge.baseRadian = 360.f;

	moveOk = TextureManager::Load("./Resources/UI/ok.png", "ok");
	cameraOk = TextureManager::Load("./Resources/UI/ok.png", "ok");
}

void MoveTutorialScene::Update()
{
	eventTimer.Update();

	GameCamera::GetInstance()->Update();

	//動いてる間
	if (Player::GetIsMove())
	{
		moveCountGauge.baseRadian -= 2.0f;
	}
	//カメラ動かしてる間
	if (Player::GetIsCameraMove())
	{
		cameraCountGauge.baseRadian -= 3.0f;
	}

	moveCountGauge.baseRadian = Util::Clamp(moveCountGauge.baseRadian, 0.0f, 360.f);
	cameraCountGauge.baseRadian = Util::Clamp(cameraCountGauge.baseRadian, 0.0f, 360.f);

	if (moveCountGauge.baseRadian <= 0.0f && cameraCountGauge.baseRadian <= 0.0f) {
		Camera::sNowCamera = nullptr;
		isActive = false;
	}

	moveCountGauge.ImGui();
	cameraCountGauge.ImGui();

	moveCountGauge.Update();
	cameraCountGauge.Update();

	moveOk.mTransform.position = moveCountGauge.baseTrans.position;
	moveOk.mTransform.position.x += 100;

	moveOk.mTransform.UpdateMatrix();
	moveOk.TransferBuffer();
	
	cameraOk.mTransform.position = cameraCountGauge.baseTrans.position;
	cameraOk.mTransform.position.x += 100;

	cameraOk.mTransform.UpdateMatrix();
	cameraOk.TransferBuffer();
}

void MoveTutorialScene::Draw()
{
	InstantDrawer::DrawGraph(
		tutorialUIPos.x, tutorialUIPos.y,
		0.5f, 0.5f, 0.f, TextureManager::Load("./Resources/UI/tutorial_move.png", "tutorial_move"));

	moveCountGauge.Draw();
	cameraCountGauge.Draw();
	
	if (moveCountGauge.baseRadian <= 0.0f) {
		moveOk.Draw();
	}
	if (cameraCountGauge.baseRadian <= 0.0f) {
		cameraOk.Draw();
	}
}
