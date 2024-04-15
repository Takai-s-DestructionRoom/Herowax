#include "BossAppearanceScene.h"
#include "InstantDrawer.h"
#include "Boss.h"

BossAppearanceScene::~BossAppearanceScene()
{
	TextureManager::Load("./Resources/exclametion.png", "exclametion");
	eventTimer = 5.f;
}

void BossAppearanceScene::Init(const Vector3 target)
{
	camera.mViewProjection.mEye = { 0, 70, -100 };
	camera.mViewProjection.mTarget = target;
	camera.mViewProjection.UpdateMatrix();

	Camera::sNowCamera = &camera;
	eventTimer.Start();

	isActive = true;
}

void BossAppearanceScene::Update()
{
	eventTimer.Update();
	if (eventTimer.GetEnd())
	{
		Camera::sNowCamera = nullptr;
		isActive = false;
	}

	camera.Update();
}

void BossAppearanceScene::Draw()
{
	InstantDrawer::DrawGraph(640.f,360.f,5.f,5.f,0.f,"exclametion");
}
