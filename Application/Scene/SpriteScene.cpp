#include "SpriteScene.h"
#include "RImGui.h"

SpriteScene::SpriteScene()
{
	sprite.SetTexture(TextureManager::Load("./Resources/conflict.jpg", "conflict"));
}

void SpriteScene::Init()
{
	Camera::sNowCamera = nullptr;
	LightGroup::sNowLight = nullptr;
}

void SpriteScene::Update()
{
	//テストGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 400 }, ImGuiCond_FirstUseEver);

		ImGui::Begin("Sprite Control");

		if (ImGui::Button("Reset")) {
			pos[0] = RWindow::GetWidth() / 2.0f;
			pos[1] = RWindow::GetHeight() / 2.0f;
			rot[0] = 0;
			rot[1] = 0;
			rot[2] = 0;
			scale[0] = 1;
			scale[1] = 1;
			anchor = { 0.5f, 0.5f };
			srcX = 0;
			srcY = 0;
			width = 1000;
			height = 1000;

			sprite.SetAnchor(anchor);
			sprite.SetTexRect(srcX, srcY, width, height);
		}

		ImGui::Text("Transform");
		ImGui::DragFloat2("Position", pos);
		ImGui::DragFloat3("Rotation", rot);
		ImGui::DragFloat2("Scale", scale, 0.01f);

		ImGui::NewLine();

		ImGui::Text("Anchor");
		if (ImGui::SliderFloat("X##AnchorX", &anchor.x, 0.0f, 1.0f)) {
			sprite.SetAnchor(anchor);
		}
		if (ImGui::SliderFloat("Y##AnchorY", &anchor.y, 0.0f, 1.0f)) {
			sprite.SetAnchor(anchor);
		}

		ImGui::NewLine();

		ImGui::Text("Rect");
		if (ImGui::SliderInt("X##SrcX", &srcX, 0, 999)) {
			width = Util::Clamp(width, 0, 1000 - srcX);
			sprite.SetTexRect(srcX, srcY, width, height);
		}
		if (ImGui::SliderInt("Y##SrcY", &srcY, 0, 999)) {
			height = Util::Clamp(height, 0, 1000 - srcY);
			sprite.SetTexRect(srcX, srcY, width, height);
		}
		if (ImGui::SliderInt("X##Width", &width, 0, 1000 - srcX)) {
			sprite.SetTexRect(srcX, srcY, width, height);
		}
		if (ImGui::SliderInt("Y##Height", &height, 0, 1000 - srcY)) {
			sprite.SetTexRect(srcX, srcY, width, height);
		}
		ImGui::End();
	}

	sprite.mTransform.position = { pos[0], pos[1], 0.0f};
	sprite.mTransform.rotation = { Util::AngleToRadian(rot[0]), Util::AngleToRadian(rot[1]), Util::AngleToRadian(rot[2]) };
	sprite.mTransform.scale = { scale[0], scale[1], 1.0f };
	sprite.mTransform.UpdateMatrix();
	sprite.TransferBuffer();
}

void SpriteScene::Draw()
{
	sprite.Draw();
}
