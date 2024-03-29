#include "TitleScene.h"
#include "ProtoScene.h"
#include "RInput.h"
#include "SceneManager.h"
#include "ImGui.h"

TitleScene::TitleScene():isPostEffect(true)
{
	titleLogo.SetTexture(TextureManager::Load("./Resources/title.png", "title"));

	camera.mViewProjection.mEye = { 0, 0, -30 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();
}

void TitleScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;

	titleLogo.SetAnchor({ 0.f, 0.f });
	//titleLogo.mTransform.scale = { 2.f, 2.f,1.f };

	obj = ModelObj(Model::Load("./Resources/Model/player/player_bird.obj", "player_bird", true));
}

void TitleScene::Update()
{
	Vector3 cameraVec = { 0, 0, 1 };
	//カメラアングル適応
	cameraVec *= Quaternion::AngleAxis(Vector3(1, 0, 0).Cross(cameraVec), cameraAngle.y);
	cameraVec *= Quaternion::AngleAxis(Vector3(0, 1, 0).Cross(cameraVec), cameraAngle.x);
	//カメラの距離適応
	cameraVec *= cameraDist;

	//プレイヤーと一定の距離を保って着いていく
	camera.mViewProjection.mEye = obj.mTransform.position + cameraVec;
	//プレイヤーの方向いてくれる
	camera.mViewProjection.mTarget = obj.mTransform.position;
	camera.mViewProjection.UpdateMatrix();

	camera.mViewProjection.UpdateMatrix();

	light.Update();

	titleLogo.mTransform.UpdateMatrix();
	titleLogo.TransferBuffer();

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(camera.mViewProjection);

	//F6かメニューボタン押されたらプロトシーンへ
	if (RInput::GetInstance()->GetKeyDown(DIK_F6) ||
		RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_START))
	{
		SceneManager::GetInstance()->Change<ProtoScene>();
	}

#pragma region ImGui
	ImGui::SetNextWindowSize({ 200, 100 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("PostEffect", NULL, window_flags);

	ImGui::Checkbox("ポストエフェクトかけるか",&isPostEffect);

	ImGui::End();

	// カメラ //
	ImGui::SetNextWindowSize({ 300, 200 });
	ImGui::Begin("Camera", NULL, window_flags);

	ImGui::Text("座標:%f,%f,%f",
		camera.mViewProjection.mEye.x,
		camera.mViewProjection.mEye.y,
		camera.mViewProjection.mEye.z);
	ImGui::SliderFloat("カメラ距離:%f", &cameraDist, -500.f, 0.f);
	ImGui::SliderAngle("カメラアングルX:%f", &cameraAngle.x);
	ImGui::SliderAngle("カメラアングルY:%f", &cameraAngle.y);

	static bool changeCamera = false;
	static float saveDist = cameraDist;
	static Vector2 saveAngle = cameraAngle;

	ImGui::End();
#pragma endregion
}

void TitleScene::Draw()
{
	titleLogo.Draw();
	obj.Draw();

	if (isPostEffect)
	{
		//bloom.Draw();
		//crossFilter.Draw();
		distortion.Draw();
	}
}
