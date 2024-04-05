#include "PlayerUI.h"
#include "Player.h"
#include "InstantDrawer.h"
#include "Texture.h"
#include "ImGui.h"
#include "Renderer.h"
#include "Minimap.h"

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

	iconSize = { 0.3f,0.3f };
	minimapIcon.SetTexture(TextureManager::Load("./Resources/minimap_icon.png", "minimapIcon"));
	minimapIcon.mMaterial.mColor = Color::kWhite;
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
	screenPos = Minimap::GetInstance()->GetScreenPos(player->obj.mTransform.position);
	minimapIcon.mTransform.position = { screenPos.x,screenPos.y,0.f };
	//回転適用
	minimapIcon.mTransform.rotation.z =
		player->obj.mTransform.rotation.y;
	//決めたサイズに
	minimapIcon.mTransform.scale = { iconSize.x,iconSize.y,1.f };

	//更新忘れずに
	minimapIcon.mTransform.UpdateMatrix();
	minimapIcon.TransferBuffer();
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

	//描画範囲制限
	Renderer::SetScissorRects({ Minimap::GetInstance()->rect });
	minimapIcon.Draw();

	//元の範囲に戻してあげる
	Renderer::SetScissorRects({ Minimap::GetInstance()->defaultRect });
}
