#include "BossUI.h"
#include "Boss.h"
#include "InstantDrawer.h"
#include "Texture.h"
#include "ImGui.h"
#include "Renderer.h"
#include "Minimap.h"

void BossUI::LoadResource()
{
	TextureManager::Load("./Resources/white2x2.png", "white2x2");
}

BossUI::BossUI()
{
	iconSize = { 10.f,10.f };
	minimapIcon.SetTexture(TextureManager::Load("./Resources/white2x2.png", "white2x2"));
	minimapIcon.mMaterial.mColor = Color::kGreen;
}

void BossUI::Update(Boss* boss)
{
	//スクリーン座標を求める
	screenPos = Minimap::GetInstance()->GetScreenPos(boss->obj.mTransform.position);
	minimapIcon.mTransform.position = { screenPos.x,screenPos.y,0.f };
	//回転適用
	minimapIcon.mTransform.rotation.z =
		boss->obj.mTransform.rotation.y;
	//決めたサイズに
	minimapIcon.mTransform.scale = { iconSize.x,iconSize.y,1.f };

	//更新忘れずに
	minimapIcon.mTransform.UpdateMatrix();
	minimapIcon.TransferBuffer();
}

void BossUI::Draw()
{
	//描画範囲制限
	Renderer::SetScissorRects({ Minimap::GetInstance()->rect });
	minimapIcon.Draw();

	//元の範囲に戻してあげる
	Renderer::SetScissorRects({ Minimap::GetInstance()->defaultRect });
}
