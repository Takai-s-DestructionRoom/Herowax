#include "ResultScene.h"
#include "TitleScene.h"
#include "RInput.h"
#include "SceneManager.h"
#include "SimpleSceneTransition.h"
#include "LightObject.h"
#include "WaxSceneTransition.h"

ResultScene::ResultScene()
{
	ResultLogo.SetTexture(TextureManager::Load("./Resources/UI/clear.png", "clear"));
}

void ResultScene::Init()
{
	Camera::sNowCamera = nullptr;
	LightGroup::sNowLight = nullptr;

	ResultLogo.SetAnchor({ 0.f, 0.f });
	
	SpotLightManager::GetInstance()->Init(&light);
}

void ResultScene::Update()
{
	//F6かメニューボタン押されたらタイトルシーンへ
	if ((RInput::GetInstance()->GetKeyDown(DIK_F6) && Util::debugBool) ||
		RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_START))
	{
		SceneManager::GetInstance()->Change<TitleScene, WaxSceneTransition>();
	}

	ResultLogo.mTransform.UpdateMatrix();
	ResultLogo.TransferBuffer();

	camera.mViewProjection.UpdateMatrix();

	light.Update();
	
	SpotLightManager::GetInstance()->Imgui();
	SpotLightManager::GetInstance()->Update();
}

void ResultScene::Draw()
{
	ResultLogo.Draw();
	SpotLightManager::GetInstance()->Draw();

}
