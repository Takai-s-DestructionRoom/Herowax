#include "ResultScene.h"
#include "TitleScene.h"
#include "RInput.h"
#include "SceneManager.h"

ResultScene::ResultScene()
{
	ResultLogo.SetTexture(TextureManager::Load("./Resources/clear.png", "clear"));
}

void ResultScene::Init()
{
	Camera::sNowCamera = nullptr;
	LightGroup::sNowLight = nullptr;

	ResultLogo.SetAnchor({ 0.f, 0.f });
}

void ResultScene::Update()
{
	//エンターかメニューボタン押されたらタイトルシーンへ
	if (RInput::GetInstance()->GetKeyDown(DIK_RETURN) ||
		RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_START))
	{
		SceneManager::GetInstance()->Change<TitleScene>();
	}

	ResultLogo.mTransform.UpdateMatrix();
	ResultLogo.TransferBuffer();

	camera.mViewProjection.UpdateMatrix();

	light.Update();
}

void ResultScene::Draw()
{
	ResultLogo.Draw();
}
