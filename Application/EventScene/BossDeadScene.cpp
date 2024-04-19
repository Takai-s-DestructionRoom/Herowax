#include "BossDeadScene.h"
#include "SceneTrance.h"

BossDeadScene::BossDeadScene()
{
	cameraPos[0] = { 50, 70, -100 };
	cameraPos[1] = { -50, 70, -100 };
	cameraPos[2] = { 0, 10, -100 };

	cameraChangeTimer[0] = 0.5f;
	cameraChangeTimer[1] = 0.5f;
	cameraChangeTimer[2] = 0.7f;
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

	eventTimer = 4.f;
	waitTimer = 0.3f;

	eventTimer.Start();
	waitTimer.Start();

	for (size_t i = 0; i < cameraChangeTimer.size(); i++)
	{
		cameraChangeTimer[i].Reset();
	}

	isActive = true;
}

void BossDeadScene::Update()
{
	eventTimer.Update();
	waitTimer.Update();
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

	camera.mViewProjection.UpdateMatrix();
}

void BossDeadScene::Draw()
{
}
