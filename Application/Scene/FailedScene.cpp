#include "FailedScene.h"
#include "TitleScene.h"
#include "ProtoScene.h"
#include "RInput.h"
#include "SceneManager.h"
#include "InstantDrawer.h"
#include "SimpleSceneTransition.h"
#include "Boss.h"
#include "Level.h"
#include <RAudio.h>

FailedScene::FailedScene()
{
	TextureManager::Load("./Resources/UI/failed.png", "failed");

	skydome = ModelObj(Model::Load("./Resources/Model/bg/bg.obj", "bg"));
	skydome.mTransform.scale = { 1.5f, 1.5f, 1.5f };
	skydome.mTransform.UpdateMatrix();

	bossTarget = ModelObj(Model::Load("./Resources/Model/hoge.obj", "hoge"));
	bossTarget.mTransform.position = { 100.f,0.f,-150.f };
	bossTarget.mTransform.UpdateMatrix();

	InstantDrawer::PreCreate();

	floatingTimer = 1.f;
	flashingTimer = 1.f;

	cameraDist = 130.f;
	cameraRot = 0.f;
	cameraRotTimer = 15.f;

	failedPos = { RWindow::GetWidth() * 0.5f,RWindow::GetHeight() * 0.5f - 150.f };
	buttonUIPos = failedPos;
	buttonUIPos.y += 400.f;

	RAudio::Load("Resources/Sounds/SE/A_select.wav", "Select");
}

void FailedScene::Init()
{
	camera.mViewProjection.mEye = { 0, 60, -cameraDist };
	camera.mViewProjection.mTarget = { 0, 30, 0 };
	camera.mViewProjection.UpdateMatrix();

	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;

	floatingTimer.Reset();
	flashingTimer.Reset();

	Boss::GetInstance()->SetTarget(&bossTarget);
}

void FailedScene::Update()
{
	InstantDrawer::DrawInit();
	floatingTimer.RoopReverse();
	flashingTimer.Roop();
	cameraRotTimer.Roop();

	Vector2 vec2;	//カメラに足すベクトル
	//円運動させる
	vec2 = Util::CircleMotion(
		{ camera.mViewProjection.mTarget.x,
		camera.mViewProjection.mTarget.z },
		cameraDist,
		cameraRotTimer.GetTimeRate() * Util::PI2);
	camera.mViewProjection.mEye = { vec2.x,60.f,vec2.y };

	Boss::GetInstance()->Update();
	Level::Get()->Update();

	//F6かメニューボタン押されたらタイトルシーンへ
	bool button = RInput::GetInstance()->GetKeyDown(DIK_F6) ||
		RInput::GetInstance()->GetKeyDown(DIK_SPACE) ||
		RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A);

	if (button && !sceneChange)
	{
		RAudio::Play("Select", 0.6f);
		SceneManager::GetInstance()->Change<TitleScene, SimpleSceneTransition>();
		sceneChange = true;
	}

	camera.mViewProjection.UpdateMatrix();

	light.Update();

	skydome.TransferBuffer(Camera::sNowCamera->mViewProjection);
	bossTarget.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void FailedScene::Draw()
{
	skydome.Draw();

	Level::Get()->Draw();
	Boss::GetInstance()->Draw();

	InstantDrawer::DrawGraph(
		failedPos.x,
		failedPos.y + Easing::InQuad(floatingTimer.GetTimeRate()) * 15.f,
		1.f, 1.f, 0.f, "failed");

	if (flashingTimer.GetTimeRate() > 0.8f)
	{
		InstantDrawer::DrawGraph(
			buttonUIPos.x, buttonUIPos.y,
			0.8f, 0.8f, 0.f, TextureManager::Load("./Resources/UI/A_push.png", "AbuttonPush"));
	}
	else
	{
		InstantDrawer::DrawGraph(
			buttonUIPos.x, buttonUIPos.y,
			0.8f, 0.8f, 0.f, TextureManager::Load("./Resources/UI/A_normal.png", "Abutton"));
	}

	//更新
	InstantDrawer::AllUpdate();
	InstantDrawer::AllDraw2D();
}
