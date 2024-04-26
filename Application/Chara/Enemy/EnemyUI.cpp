#include "EnemyUI.h"
#include "Enemy.h"
#include "InstantDrawer.h"
#include "Texture.h"
#include "Renderer.h"
#include "Camera.h"
#include "Minimap.h"

void EnemyUI::LoadResource()
{
	//TextureManager::Load("./Resources/white2x2.png", "white2x2");
	/*TextureManager::Load("./Resources/minimap_icon.png", "minimapIcon");*/
}

EnemyUI::EnemyUI() //: position(1, 1, 1), size(4, 1);
{
	/*minimapIcon.SetTexture(TextureManager::Load("./Resources/minimap_icon.png", "minimapIcon"));
	minimapIcon.mMaterial.mColor = Color::kRed;
	minimapIcon.mMaterial.mColor.a = 0.5f;*/
}

void EnemyUI::Update(Enemy* enemy)
{
	enemy;
	/*position = enemy->obj.mTransform.position;
	position.y += enemy->GetScale().y * 2;
	size.x = enemy->GetHP() / 4;
	maxSize.x = (enemy->GetMaxHP() / 4) * 1.1f;
	maxSize.y = size.y * 1.1f;*/

	////スクリーン座標を求める
	//screenPos = Minimap::GetInstance()->GetScreenPos(enemy->obj.mTransform.position);
	//minimapIcon.mTransform.position = { screenPos.x,screenPos.y,0.f };
	////回転適用
	//minimapIcon.mTransform.rotation.z =
	//	enemy->obj.mTransform.rotation.y;
	////決めたサイズに
	//iconSize = { enemy->obj.mTransform.scale.x * 0.03f,enemy->obj.mTransform.scale.z * 0.03f };
	//minimapIcon.mTransform.scale =
	//	Vector3(iconSize.x, iconSize.y, 1.f) * Minimap::GetInstance()->iconSize;

	////更新忘れずに
	//minimapIcon.mTransform.UpdateMatrix();
	//minimapIcon.TransferBuffer();
}

void EnemyUI::Draw()
{
	//Vector3 backPos = position;
	//Vector3 targetEyeVec = Camera::sNowCamera->mViewProjection.mTarget - Camera::sNowCamera->mViewProjection.mEye;
	//targetEyeVec.Normalize();
	//backPos += targetEyeVec * 0.01f;	//ここをカメラ方向へのプラスに変えればok
	//InstantDrawer::DrawGraph3D(backPos, maxSize.x, maxSize.y, "white2x2", Color::kBlack);
	//InstantDrawer::DrawGraph3D(position, size.x, size.y, "white2x2", Color::kRed);

	////描画範囲制限
	//Renderer::SetScissorRects({ Minimap::GetInstance()->rect });
	//minimapIcon.Draw();

	////元の範囲に戻してあげる
	//Renderer::SetScissorRects({ Minimap::GetInstance()->defaultRect });
}