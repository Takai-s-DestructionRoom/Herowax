#include "BossAppearanceScene.h"
#include "InstantDrawer.h"
#include "Boss.h"
#include "SceneTrance.h"

BossAppearanceScene::~BossAppearanceScene()
{
	TextureManager::Load("./Resources/boss.png", "boss");
}

void BossAppearanceScene::Init(const Vector3 target)
{
	camera.mViewProjection.mEye = { 0, 10, -100 };
	camera.mViewProjection.mTarget = target;
	camera.mViewProjection.UpdateMatrix();

	Camera::sNowCamera = &camera;

	eventTimer = 4.f;
	waitTimer = 0.5f;
	textEaseTimer = 0.8f;

	eventTimer.Start();
	waitTimer.Start();

	isActive = true;
}

void BossAppearanceScene::Update()
{
	eventTimer.Update();
	waitTimer.Update();
	textEaseTimer.Update();

	if (waitTimer.GetEnd() && textEaseTimer.GetStarted() == false)
	{
		textEaseTimer.Start();
	}

	textOriSize = { 2.5f,2.5f };
	textSize = textOriSize * Easing::OutBack(textEaseTimer.GetTimeRate());

	//イベントシーン終わったらシーン遷移開始
	if (eventTimer.GetEnd())
	{
		SceneTrance::GetInstance()->Start();
	}

	//切り替えても良くなったら切り替えして終了
	if (eventTimer.GetEnd() && SceneTrance::GetInstance()->GetIsChange())
	{
		Camera::sNowCamera = nullptr;
		isActive = false;

		SceneTrance::GetInstance()->SetIsChange(false);
	}

	camera.Update();
}

void BossAppearanceScene::Draw()
{
	InstantDrawer::DrawGraph(640.f, 550.f, textSize.x, textSize.y, 0.f, "boss");
}