#include "MoveTutorialScene.h"
#include "Player.h"
#include "InstantDrawer.h"
#include "GameCamera.h"
#include "RInput.h"
#include "RImGui.h"
#include "Parameter.h"

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
	skipCountGauge.Init("skipCountGauge");

	moveCountGauge.baseRadian = 360.f;
	cameraCountGauge.baseRadian = 360.f;
	skipCountGauge.baseRadian = 360.f;

	moveOk = TextureManager::Load("./Resources/UI/ok.png", "ok");
	cameraOk = TextureManager::Load("./Resources/UI/ok.png", "ok");

	auto extract = Parameter::Extract("Aボタン");
	AbuttonPos.x = Parameter::GetParam(extract, "Aボタン位置X", 0);
	AbuttonPos.y = Parameter::GetParam(extract, "Aボタン位置Y", 0);
	AbuttonSize.x = Parameter::GetParam(extract, "AボタンサイズX", 0);
	AbuttonSize.y = Parameter::GetParam(extract, "AボタンサイズY", 0);
}

void MoveTutorialScene::Update()
{
	eventTimer.Update();

	//GameCamera::GetInstance()->Update();

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

	moveCountGauge.baseRadian = Util::Clamp(moveCountGauge.baseRadian, 0.0f, 360.f);
	cameraCountGauge.baseRadian = Util::Clamp(cameraCountGauge.baseRadian, 0.0f, 360.f);
	skipCountGauge.baseRadian = Util::Clamp(skipCountGauge.baseRadian, 0.f, 360.f);

	if (moveCountGauge.baseRadian <= 0.0f && cameraCountGauge.baseRadian <= 0.0f ||
		skipCountGauge.baseRadian <= 0.0f)
	{
		Camera::sNowCamera = nullptr;
		isActive = false;
	}

	if (RImGui::showImGui)
	{
		ImGui::SetNextWindowSize({ 400, 200 }, ImGuiCond_FirstUseEver);

		// デバッグモード //
		ImGui::Begin("Aボタン");

		ImGui::DragFloat2("Aボタン位置", &AbuttonPos.x);
		ImGui::DragFloat2("Aボタンサイズ", &AbuttonSize.x);

		if (ImGui::Button("セーブ")) {
			Parameter::Begin("Aボタン");

			Parameter::Save("Aボタン位置X", AbuttonPos.x);
			Parameter::Save("Aボタン位置Y", AbuttonPos.y);
			Parameter::Save("AボタンサイズX", AbuttonSize.x);
			Parameter::Save("AボタンサイズY", AbuttonSize.y);
			Parameter::End();
		}
		ImGui::End();
	}

	moveCountGauge.ImGui();
	cameraCountGauge.ImGui();
	skipCountGauge.ImGui();

	moveCountGauge.Update();
	cameraCountGauge.Update();
	skipCountGauge.Update();

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

	moveCountGauge.Draw();
	cameraCountGauge.Draw();
	skipCountGauge.Draw();
	
	if (moveCountGauge.baseRadian <= 0.0f) {
		moveOk.Draw();
	}
	if (cameraCountGauge.baseRadian <= 0.0f) {
		cameraOk.Draw();
	}
}
