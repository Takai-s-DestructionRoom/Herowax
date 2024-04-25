#include "BossUI.h"
#include "Boss.h"
#include "InstantDrawer.h"
#include "Texture.h"
//#include "ImGui.h"
#include "Renderer.h"
#include "Minimap.h"

void BossUI::LoadResource()
{
	TextureManager::Load("./Resources/white2x2.png", "white2x2");
}

BossUI::BossUI()
{
	/*minimapIcon.SetTexture(TextureManager::Load("./Resources/white2x2.png", "white2x2"));
	minimapIcon.mMaterial.mColor = Color::kGreen;*/
}

void BossUI::Update(Boss* boss)
{
	boss;
	////スクリーン座標を求める
	//screenPos = Minimap::GetInstance()->GetScreenPos(boss->obj.mTransform.position);
	//minimapIcon.mTransform.position = { screenPos.x,screenPos.y,0.f };
	////回転適用
	//minimapIcon.mTransform.rotation.z =
	//	boss->obj.mTransform.rotation.y;
	////決めたサイズに
	//iconSize = { boss->obj.mTransform.scale.x * 2.5f,boss->obj.mTransform.scale.z * 2.5f };
	//minimapIcon.mTransform.scale = Vector3(iconSize.x,iconSize.y,1.f) * Minimap::GetInstance()->iconSize;

	////更新忘れずに
	//minimapIcon.mTransform.UpdateMatrix();
	//minimapIcon.TransferBuffer();
}

void BossUI::Draw()
{
	////描画範囲制限
	//Renderer::SetScissorRects({ Minimap::GetInstance()->rect });
	//minimapIcon.Draw();

	////元の範囲に戻してあげる
	//Renderer::SetScissorRects({ Minimap::GetInstance()->defaultRect });
}
