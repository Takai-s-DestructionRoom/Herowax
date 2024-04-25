#include "TitleScene.h"
#include "ProtoScene.h"
#include "RInput.h"
#include "SceneManager.h"
#include "InstantDrawer.h"
#include "SimpleSceneTransition.h"
#include <RAudio.h>

TitleScene::TitleScene()
{
	TextureManager::Load("./Resources/UI/hi-rou_logo_eye.png", "title");

	skydome = ModelObj(Model::Load("./Resources/Model/bg/bg.obj", "bg"));
	skydome.mTransform.scale = { 1.5f, 1.5f, 1.5f };
	skydome.mTransform.UpdateMatrix();

	InstantDrawer::PreCreate();

	Level::Get()->Load();

	floatingTimer = 1.f;
	flashingTimer = 1.f;

	cameraDist = 100.f;
	cameraRot = 0.f;
	cameraRotTimer = 10.f;

	titleLogoPos = { RWindow::GetWidth() * 0.5f,RWindow::GetHeight() * 0.5f - 100.f };
	buttonUIPos = titleLogoPos;
	buttonUIPos.y += 300.f;

	RAudio::Load("Resources/Sounds/SE/A_select.wav", "Select");
}

void TitleScene::Init()
{
	camera.mViewProjection.mEye = { 0, 30, -cameraDist };
	camera.mViewProjection.mTarget = { 0, 10, 0 };
	camera.mViewProjection.UpdateMatrix();

	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;

	//とりあえず最初のステージを設定しておく
	Level::Get()->Extract("test");

	floatingTimer.Reset();
	flashingTimer.Reset();
	cameraRotTimer.Reset();
}

void TitleScene::Update()
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
	camera.mViewProjection.mEye = { vec2.x,30.f,vec2.y };

	Level::Get()->Update();

	//F6かメニューボタン押されたらプロトシーンへ
	bool button = /*RInput::GetInstance()->GetKeyDown(DIK_F6) ||*/
		RInput::GetInstance()->GetKeyDown(DIK_SPACE) ||
		RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A);
	if (button && !sceneChange)
	{
		sceneChange = true;
		RAudio::Play("Select", 0.6f);
		SceneManager::GetInstance()->Change<ProtoScene, SimpleSceneTransition>();
	}

	camera.mViewProjection.UpdateMatrix();

	light.Update();

	skydome.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void TitleScene::Draw()
{
	skydome.Draw();

	Level::Get()->Draw();

	InstantDrawer::DrawGraph(
		titleLogoPos.x,
		titleLogoPos.y + Easing::InQuad(floatingTimer.GetTimeRate()) * 15.f,
		1.f, 1.f, 0.f, "title");

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
