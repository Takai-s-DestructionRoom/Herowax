#include "PlayerUI.h"
#include "Player.h"
#include "InstantDrawer.h"
#include "Texture.h"

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
}
