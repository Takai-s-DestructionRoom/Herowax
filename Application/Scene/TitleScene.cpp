#include "TitleScene.h"
#include "ProtoScene.h"
#include "RInput.h"
#include "SceneManager.h"

TitleScene::TitleScene()
{
	titleLogo.SetTexture(TextureManager::Load("./Resources/title.png", "title"));
}

void TitleScene::Init()
{
	Camera::sNowCamera = nullptr;
	LightGroup::sNowLight = nullptr;

	titleLogo.SetAnchor({ 0.f, 0.f });
}

void TitleScene::Update()
{
	//F6かメニューボタン押されたらプロトシーンへ
	if (RInput::GetInstance()->GetKeyDown(DIK_F6) ||
		RInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_START))
	{
		SceneManager::GetInstance()->Change<ProtoScene>();
	}

	titleLogo.mTransform.UpdateMatrix();
	titleLogo.TransferBuffer();

	camera.mViewProjection.UpdateMatrix();

	light.Update();
}

void TitleScene::Draw()
{
	titleLogo.Draw();
}
