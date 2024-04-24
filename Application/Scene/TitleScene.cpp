#include "TitleScene.h"
#include "ProtoScene.h"
#include "RInput.h"
#include "SceneManager.h"
#include "InstantDrawer.h"

TitleScene::TitleScene()
{
	TextureManager::Load("./Resources/hi-rou_logo_eye.png", "title");
	TextureManager::Load("./Resources/Abutton_UI_normal.png", "Abutton");
	TextureManager::Load("./Resources/Abutton_UI_push.png", "AbuttonPush");

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
}

void TitleScene::Init()
{
	camera.mViewProjection.mEye = { 0, 0, -cameraDist };
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

	titleLogoPos;

	//F6かメニューボタン押されたらプロトシーンへ
	if (RInput::GetInstance()->GetKeyDown(DIK_F6) ||
		RInput::GetInstance()->GetKeyDown(DIK_SPACE) ||
		RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A))
	{
		SceneManager::GetInstance()->Change<ProtoScene>();
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

	if (floatingTimer.GetTimeRate() > 0.8f)
	{
		InstantDrawer::DrawGraph(
			buttonUIPos.x, buttonUIPos.y,
			0.8f, 0.8f, 0.f, "AbuttonPush");
	}
	else
	{
		InstantDrawer::DrawGraph(
			buttonUIPos.x, buttonUIPos.y,
			0.8f, 0.8f, 0.f, "Abutton");
	}

	//更新
	InstantDrawer::AllUpdate();
	InstantDrawer::AllDraw2D();
}
