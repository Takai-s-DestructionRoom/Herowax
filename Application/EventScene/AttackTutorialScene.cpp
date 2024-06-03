#include "AttackTutorialScene.h"
#include "Player.h"
#include "InstantDrawer.h"
#include "GameCamera.h"
#include "EventCaller.h"
#include "RInput.h"
#include "Parameter.h"
#include "RAudio.h"

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
	RAudio::Load("Resources/Sounds/SE/A_select.wav", "Select");
	Camera::sNowCamera = &GameCamera::GetInstance()->camera;

	eventTimer.Start();

	isActive = true;

	tutorialUIPos = { RWindow::GetWidth() * 0.5f,RWindow::GetHeight() * 0.5f - 100.f };
	tutorialUIPos.y += 300.f;

	attackCountGauge.Init("attackCountGauge");
	collectCountGauge.Init("collectCountGauge");
	skipCountGauge.Init("skipCountGauge");

	attackCountGauge.baseRadian = 360.f;
	collectCountGauge.baseRadian = 360.f;
	skipCountGauge.baseRadian = 360.f;

	attackOk = TextureManager::Load("./Resources/UI/ok.png", "ok");
	collectOk = TextureManager::Load("./Resources/UI/ok.png", "ok");

	auto extract = Parameter::Extract("Aボタン");
	AbuttonPos.x = Parameter::GetParam(extract, "Aボタン位置X", 0);
	AbuttonPos.y = Parameter::GetParam(extract, "Aボタン位置Y", 0);
	AbuttonSize.x = Parameter::GetParam(extract, "AボタンサイズX", 0);
	AbuttonSize.y = Parameter::GetParam(extract, "AボタンサイズY", 0);
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
	//Aボタンかエンターキー押してる間
	if (RInput::GetInstance()->GetPadButton(XINPUT_GAMEPAD_A) || RInput::GetKey(DIK_RETURN))
	{
		skipCountGauge.baseRadian -= 4.0f;
		isPush = true;
	}
	else
	{
		skipCountGauge.baseRadian += 8.f;
		isPush = false;
	}

	attackCountGauge.baseRadian = Util::Clamp(attackCountGauge.baseRadian, 0.0f, 360.f);
	collectCountGauge.baseRadian = Util::Clamp(collectCountGauge.baseRadian, 0.0f, 360.f);
	skipCountGauge.baseRadian = Util::Clamp(skipCountGauge.baseRadian, 0.f, 360.f);

	if ((attackCountGauge.baseRadian <= 0.0f && collectCountGauge.baseRadian <= 0.0f) ||
		skipCountGauge.baseRadian <= 0.0f)
	{
		EventCaller::saveCamera = Camera::sNowCamera;
		RAudio::Play("Select", 1.4f);
		Camera::sNowCamera = nullptr;
		isActive = false;
	}

	attackCountGauge.ImGui();
	collectCountGauge.ImGui();
	skipCountGauge.ImGui();

	attackCountGauge.Update();
	collectCountGauge.Update();
	skipCountGauge.Update();

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

	if (isPush)
	{
		InstantDrawer::DrawGraph(
			AbuttonPos.x, AbuttonPos.y,
			AbuttonSize.x, AbuttonSize.y, 0.f, TextureManager::Load("./Resources/UI/A_push.png", "A_push"));
	}
	else
	{
		InstantDrawer::DrawGraph(
			AbuttonPos.x, AbuttonPos.y,
			AbuttonSize.x, AbuttonSize.y, 0.f, TextureManager::Load("./Resources/UI/A_normal.png", "A_normal"));
	}

	InstantDrawer::DrawGraph(
		AbuttonPos.x - 150.f, AbuttonPos.y,
		0.8f, 0.8f, 0.f, TextureManager::Load("./Resources/UI/skipText.png", "skip"));

	attackCountGauge.Draw();
	collectCountGauge.Draw();
	skipCountGauge.Draw();

	if (attackCountGauge.baseRadian <= 0.0f) {
		attackOk.Draw();
	}
	if (collectCountGauge.baseRadian <= 0.0f) {
		collectOk.Draw();
	}
}
