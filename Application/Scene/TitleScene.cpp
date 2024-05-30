#include "TitleScene.h"
#include "ProtoScene.h"
#include "RInput.h"
#include "SceneManager.h"
#include "ParticleManager.h"
#include "InstantDrawer.h"
#include "SimpleSceneTransition.h"
#include "WaxSceneTransition.h"
#include <RAudio.h>
#include "LightObject.h"
#include "RImGui.h"
#include "Parameter.h"

TitleScene::TitleScene()
{
	TextureManager::Load("./Resources/UI/hi-rou_logo_eye.png", "title");
	TextureManager::Load("./Resources/UI/controller.png", "controller");

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
	RAudio::Load("Resources/Sounds/BGM/Gametitle.wav", "Title");

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

	ParticleManager::GetInstance()->Init();

	SpotLightManager::GetInstance()->Init(&light);

	RAudio::Stop("Boss");

	RAudio::Play("Title", 0.4f, 1.0f, true);
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

	ParticleManager::GetInstance()->Update();

	//F6かメニューボタン押されたらプロトシーンへ
	bool button = RInput::GetInstance()->GetKeyDown(DIK_F6) ||
		RInput::GetInstance()->GetKeyDown(DIK_SPACE) ||
		RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A);
	if (button && !sceneChange)
	{
		sceneChange = true;
		RAudio::Play("Select", 0.6f);
		SceneManager::GetInstance()->Change<ProtoScene, SimpleSceneTransition>();
		//SceneManager::GetInstance()->Change<ProtoScene, WaxSceneTransition>();
		RAudio::Stop("Title");
	}

	camera.mViewProjection.UpdateMatrix();

	light.Update();

	skydome.TransferBuffer(Camera::sNowCamera->mViewProjection);

	SpotLightManager::GetInstance()->Imgui();
	SpotLightManager::GetInstance()->Update();

	if (RImGui::showImGui) {
		ImGui::Begin("ブラシ");
		if (ImGui::Button("作成")) {
			brushs.emplace_back();
			brushs.back().Init();
			brushs.back().Start({0.f,0.f},{1.0f,1.0f});
		}
		int32_t i = 0;
		for (auto& brush : brushs)
		{
			std::string string = "brush_pos_";
			string += std::to_string(i);
			ImGui::DragFloat3(string.c_str(), &brush.sprite.mTransform.position.x);
			string = "brush_scale_";
			string += std::to_string(i);
			ImGui::DragFloat3(string.c_str(), &brush.sprite.mTransform.scale.x,0.01f);
			i++;
		}
		if (ImGui::Button("セーブ")) {
			Parameter::Begin("WaxSceneTransition");
			i = 0;
			for (auto& brush : brushs)
			{
				std::string string = "pos_";
				string += std::to_string(i);
				Parameter::SaveVector3(string.c_str(), brush.sprite.mTransform.position);
				string = "scale_";
				string += std::to_string(i);
				Parameter::SaveVector3(string.c_str(), brush.sprite.mTransform.scale);
				i++;
			}
			Parameter::End();
		}
		ImGui::End();
	}
	for (auto& brush : brushs)
	{
		brush.Update();
	}
}

void TitleScene::Draw()
{
	skydome.Draw();
	SpotLightManager::GetInstance()->Draw();

	Level::Get()->Draw();

	
	InstantDrawer::DrawGraph(
		titleLogoPos.x,
		titleLogoPos.y + Easing::InQuad(floatingTimer.GetTimeRate()) * 15.f,
		1.f, 1.f, 0.f, "title");
	
	/*InstantDrawer::DrawGraph(
		320.f,
		80.f,
		0.6f, 0.6f, 0.f, "controller");*/

	if (flashingTimer.GetTimeRate() > 0.8f)
	{
		InstantDrawer::DrawGraph(
			buttonUIPos.x, buttonUIPos.y,
			0.12f, 0.12f, 0.f, TextureManager::Load("./Resources/UI/A_push.png", "AbuttonPush"));
	}
	else
	{
		InstantDrawer::DrawGraph(
			buttonUIPos.x, buttonUIPos.y,
			0.12f, 0.12f, 0.f, TextureManager::Load("./Resources/UI/A_normal.png", "Abutton"));
	}

	ParticleManager::GetInstance()->Draw();

	//更新
	InstantDrawer::AllUpdate();
	InstantDrawer::AllDraw2D();

	for (auto& brush : brushs)
	{
		brush.Draw();
	}
}
