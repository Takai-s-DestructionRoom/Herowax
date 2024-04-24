#include "BossDeadScene.h"
#include "SceneTrance.h"
#include "InstantDrawer.h"

BossDeadScene::BossDeadScene()
{
	cameraPos[0] = { 50, 70, -100 };
	cameraPos[1] = { -50, 70, -100 };
	cameraPos[2] = { 0, 10, -100 };

	cameraChangeTimer[0] = 0.5f;
	cameraChangeTimer[1] = 0.5f;
	cameraChangeTimer[2] = 0.7f;

	TextureManager::Load("./Resources/clear.png", "clear");
	clearStrPos = { RWindow::GetWidth() * 0.5f,- 300.f };
}

BossDeadScene::~BossDeadScene()
{
}

void BossDeadScene::Init(const Vector3 target)
{
	camera.mViewProjection.mEye = cameraPos[0];
	camera.mViewProjection.mTarget = target;
	camera.mViewProjection.UpdateMatrix();

	Camera::sNowCamera = &camera;

	clearStrTimer = 0.5f;
	eventTimer = 5.0f;
	floatingTimer = 1.0f;
	waitTimer = 0.3f;
	
	eventTimer.Start();
	waitTimer.Start();
	clearStrTimer.Reset();
	floatingTimer.Reset();
	
	for (size_t i = 0; i < cameraChangeTimer.size(); i++)
	{
		cameraChangeTimer[i].Reset();
	}

	isActive = true;
	callStr = false;
}

void BossDeadScene::Update()
{
	eventTimer.Update();
	waitTimer.Update();
	clearStrTimer.Update();
	
	//タイマーが終わったらループ
	if (clearStrTimer.GetEnd()) {
		floatingTimer.RoopReverse();
	}
	clearStrPos.y = Easing::OutQuad(-500.f, RWindow::GetHeight() * 0.5f, clearStrTimer.GetTimeRate());

	for (size_t i = 0; i < cameraChangeTimer.size(); i++)
	{
		cameraChangeTimer[i].Update();
	}

	if (waitTimer.GetEnd() && cameraChangeTimer[0].GetStarted() == false)
	{
		cameraChangeTimer[0].Start();
	}

	//最初のタイマー以降は終わるまで続いてく
	for (size_t i = 1; i < cameraChangeTimer.size(); i++)
	{
		if (cameraChangeTimer[i-1].GetEnd() && cameraChangeTimer[i].GetStarted() == false)
		{
			cameraChangeTimer[i].Start();
			camera.mViewProjection.mEye = cameraPos[i];
		}
	}

	//爆発するタイミングに当たりをつけてクリア文字列を呼ぶ
	if (eventTimer.GetTimeRate() > 0.7f && !callStr)
	{
		clearStrTimer.Start();
		callStr = true;
	}

	//イベントシーン終わったらシーン遷移開始(待機を追加)
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

	camera.mViewProjection.UpdateMatrix();
}

void BossDeadScene::Draw()
{
	InstantDrawer::DrawGraph(
		clearStrPos.x,
		clearStrPos.y + Easing::InQuad(floatingTimer.GetTimeRate()) * 15.f,
		1.f, 1.f, 0.f, "clear");
}
