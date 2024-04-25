#include "Minimap.h"
#include "Parameter.h"
#include "RWindow.h"
#include "RWindow.h"
#include "InstantDrawer.h"
#include "Camera.h"
#include "ImGui.h"

Minimap::Minimap()
{
	frameColor = Color::kWhite;
	backColor = Color::kBlack;
	backColor.a = 0.5f;

	backSprite.SetTexture(TextureManager::Load("./Resources/minimap_back.png", "minimapBack"));
	backSprite.mMaterial.mColor = Color::kWhite;
	backSprite.mMaterial.mColor.a = 0.5f;

	defaultRect = {
		0,static_cast<long>(RWindow::GetWidth()),
		0,  static_cast<long>(RWindow::GetHeight())
	};
}

Minimap* Minimap::GetInstance()
{
	static Minimap instance;
	return &instance;
}

void Minimap::Init()
{
	std::map<std::string, std::string> extract = Parameter::Extract("Minimap");
	pos.x = Parameter::GetParam(extract, "ミニマップ座標X", 125.f);
	pos.y = Parameter::GetParam(extract, "ミニマップ座標Y", 595.f);
	size = Parameter::GetParam(extract, "ミニマップサイズ", 150.f);

	rect = { (long)(pos.x - size * 0.5f), (long)(pos.x + size * 0.5f),
		(long)(pos.y - size * 0.5f), (long)(pos.y + size * 0.5f) };
}

void Minimap::Update()
{
	backSprite.mTransform.position = {
		pos.x,pos.y,0.f };
	backSprite.mTransform.scale = { size/ 300.f,size/ 300.f,1.f };

	backSprite.mTransform.UpdateMatrix();
	backSprite.TransferBuffer();

	rect = { (long)(pos.x - size * 0.5f), (long)(pos.x + size * 0.5f),
		(long)(pos.y - size * 0.5f), (long)(pos.y + size * 0.5f) };

	//カメラの距離とミニマップのサイズを基準にアイコン倍率設定
	float cameraDist = Camera::sMinimapCamera->mViewProjection.mEye.y;
	iconSize = 1.f / (cameraDist / 250.f) * (Minimap::GetInstance()->size / 180.f);

//#pragma region ImGui
//	ImGui::SetNextWindowSize({ 300, 200 }, ImGuiCond_FirstUseEver);
//
//	// カメラ //
//	ImGui::Begin("Minimap");
//
//	ImGui::InputFloat("ミニマップ座標X", &pos.x, 1.0f);
//	ImGui::InputFloat("ミニマップ座標Y", &pos.y, 1.0f);
//	ImGui::InputFloat("ミニマップサイズ", &size, 5.0f);
//	size = Util::Clamp(size, 50.f, 500.f);
//
//	if (ImGui::Button("セーブ")) {
//		Parameter::Begin("Minimap");
//		Parameter::Save("ミニマップ座標X", pos.x);
//		Parameter::Save("ミニマップ座標Y", pos.y);
//		Parameter::Save("ミニマップサイズ", size);
//		Parameter::End();
//	}
//
//	ImGui::End();
//#pragma endregion
}

void Minimap::Draw()
{
	backSprite.Draw();

	//マップのサイズ÷テクスチャのサイズ
	InstantDrawer::DrawGraph(
		pos.x, pos.y,
		size / 320.f, size / 320.f, 0.f,
		TextureManager::Load("./Resources/minimap_frame.png", "minimapFrame"));
}

Vector2 Minimap::GetScreenPos(Vector3 wpos)
{
	Vector2 screenPos =
		Camera::sMinimapCamera->mViewProjection.WorldToScreen(
			wpos,
			(float)Minimap::GetInstance()->rect.left,
			(float)Minimap::GetInstance()->rect.top,
			Minimap::GetInstance()->size, Minimap::GetInstance()->size, 0, 1);

	return screenPos;
}
