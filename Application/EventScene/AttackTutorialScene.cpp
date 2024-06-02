#include "AttackTutorialScene.h"
#include "Player.h"
#include "InstantDrawer.h"
#include "GameCamera.h"
#include "EventCaller.h"

AttackTutorialScene::AttackTutorialScene()
{
}

AttackTutorialScene::~AttackTutorialScene()
{
}

void AttackTutorialScene::Init(const Vector3 target)
{
	camera.mViewProjection.mEye = Camera::sNowCamera->mViewProjection.mEye;
	camera.mViewProjection.mTarget = target;
	camera.mViewProjection.UpdateMatrix();

	Camera::sNowCamera = &GameCamera::GetInstance()->camera;

	eventTimer.Start();

	isActive = true;

	tutorialUIPos = { RWindow::GetWidth() * 0.5f,RWindow::GetHeight() * 0.5f - 100.f };
	tutorialUIPos.y += 300.f;

	attackCountGauge.Init("attackCountGauge");
	collectCountGauge.Init("collectCountGauge");

	attackCountGauge.baseRadian = 360.f;
	collectCountGauge.baseRadian = 360.f;

	attackOk = TextureManager::Load("./Resources/UI/ok.png", "ok");
	collectOk = TextureManager::Load("./Resources/UI/ok.png", "ok");
}

void AttackTutorialScene::Update()
{
	eventTimer.Update();

	//攻撃してる間
	if (Player::GetIsAttackTutorial())
	{
		attackCountGauge.baseRadian -= 10.0f;
	}
	//回収してる間
	if (Player::GetIsCollectTutorial())
	{
		collectCountGauge.baseRadian -= 3.0f;
	}

	attackCountGauge.baseRadian = Util::Clamp(attackCountGauge.baseRadian, 0.0f, 360.f);
	collectCountGauge.baseRadian = Util::Clamp(collectCountGauge.baseRadian, 0.0f, 360.f);

	if (attackCountGauge.baseRadian <= 0.0f && collectCountGauge.baseRadian <= 0.0f)
	{
		EventCaller::saveCamera = Camera::sNowCamera;

		Camera::sNowCamera = nullptr;
		isActive = false;
	}

	attackCountGauge.ImGui();
	collectCountGauge.ImGui();

	attackCountGauge.Update();
	collectCountGauge.Update();

	attackOk.mTransform.position = attackCountGauge.baseTrans.position;
	attackOk.mTransform.position.x += 100;

	attackOk.mTransform.UpdateMatrix();
	attackOk.TransferBuffer();

	collectOk.mTransform.position = collectCountGauge.baseTrans.position;
	collectOk.mTransform.position.x += 100;

	collectOk.mTransform.UpdateMatrix();
	collectOk.TransferBuffer();
}

void AttackTutorialScene::Draw()
{
	InstantDrawer::DrawGraph(
		tutorialUIPos.x, tutorialUIPos.y,
		0.5f, 0.5f, 0.f, TextureManager::Load("./Resources/UI/tutorial_attack.png", "tutorial_attack"));

	attackCountGauge.Draw();
	collectCountGauge.Draw();

	if (attackCountGauge.baseRadian <= 0.0f) {
		attackOk.Draw();
	}
	if (collectCountGauge.baseRadian <= 0.0f) {
		collectOk.Draw();
	}
}
