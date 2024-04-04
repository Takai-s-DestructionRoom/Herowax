#include "PlayerUI.h"
#include "Player.h"
#include "InstantDrawer.h"
#include "Texture.h"
#include "Camera.h"
#include "ImGui.h"

void PlayerUI::LoadResource()
{
	TextureManager::Load("./Resources/white2x2.png", "white2x2");
}

PlayerUI::PlayerUI()
{
	//色指定
	gaugeColor[(uint32_t)UIType::waxGauge] = Color::kWhite;
	gaugeColor[(uint32_t)UIType::fireGauge] = Color::kRed;

	//サイズ指定
	for (uint32_t i = 0; i < (uint32_t)UIType::max; i++)
	{
		size[i] = { 5.f,0.2f };
		maxSize[i] = size[i] * 1.1f;
	}

	iconSize = { 5.f,5.f };
	iconColor = Color::kWhite;
}

void PlayerUI::Update(Player* player)
{
	//座標を合わせ続ける
	for (uint32_t i = 0; i < (uint32_t)UIType::max; i++)
	{
		position[i] = player->obj.mTransform.position;
		position[i].y += player->GetScale().y * 4.f + (float)(i) * 0.5f;
	}

	//ゲージの割合を指定されたサイズとかける
	size[(uint32_t)UIType::waxGauge].x =
		maxSize[(uint32_t)UIType::waxGauge].x * ((float)player->waxStock / (float)player->maxWaxStock);

	size[(uint32_t)UIType::fireGauge].x =
		maxSize[(uint32_t)UIType::fireGauge].x * player->fireUnit.fireGauge / player->fireUnit.maxFireGauge;

	//スクリーン座標を求める
	screenPos =
		Camera::sMinimapCamera->mViewProjection.WorldToScreen(
			player->obj.mTransform.position,
			50.f, static_cast<float>(RWindow::GetHeight()) - 200.f,
			100.f, 100.f, 0, 1);

	ImGui::SetNextWindowSize({ 350, 100 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	// カメラ //
	ImGui::Begin("MinimapIcon", NULL, window_flags);

	ImGui::Text("座標:%f,%f",
		screenPos.x,
		screenPos.y);

	ImGui::Text("ワールド座標:%f,%f,%f",
		player->obj.mTransform.position.x,
		player->obj.mTransform.position.y,
		player->obj.mTransform.position.z);

	ImGui::End();
}

void PlayerUI::Draw()
{
	for (uint32_t i = 0; i < (uint32_t)UIType::max; i++)
	{
		Vector3 backPos = position[i];
		backPos.z += 0.01f;
		InstantDrawer::DrawGraph3D(backPos, maxSize[i].x, maxSize[i].y, "white2x2", Color::kBlack);
		InstantDrawer::DrawGraph3D(position[i], size[i].x, size[i].y, "white2x2", gaugeColor[i]);
	}

	InstantDrawer::DrawGraph(screenPos.x, screenPos.y,
		iconSize.x, iconSize.y, 0, "white2x2", iconColor);
}
