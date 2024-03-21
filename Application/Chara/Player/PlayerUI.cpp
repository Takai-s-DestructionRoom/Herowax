#include "PlayerUI.h"
#include "Player.h"
#include "InstantDrawer.h"
#include "Texture.h"

void PlayerUI::LoadResource()
{
	TextureManager::Load("./Resources/white2x2.png", "white2x2");
}

PlayerUI::PlayerUI() : position(1, 1, 1), size(5.f, 0.2f)
{
}

void PlayerUI::Update(Player* player)
{
	position = player->obj.mTransform.position;
	position.y += player->GetScale().y * 1.8f;
	size.x = player->fireGauge;
	maxSize.x = player->maxFireGauge * 1.1f;
	maxSize.y = size.y * 1.1f;
}

void PlayerUI::Draw()
{
	Vector3 backPos = position;
	backPos.z += 0.01f;
	InstantDrawer::DrawGraph3D(backPos, maxSize.x, maxSize.y, "white2x2", Color::kBlack);
	InstantDrawer::DrawGraph3D(position, size.x, size.y, "white2x2", Color::kRed);
}
