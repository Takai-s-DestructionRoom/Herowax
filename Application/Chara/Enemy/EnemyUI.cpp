#include "EnemyUI.h"
#include "Enemy.h"
#include "InstantDrawer.h"
#include "Texture.h"
#include "Camera.h"
#include "Renderer.h"

void EnemyUI::LoadResource()
{
	TextureManager::Load("./Resources/white2x2.png", "white2x2");
	TextureManager::Load("./Resources/minimap_icon.png", "minimapIcon");
}

EnemyUI::EnemyUI() : position(1,1,1),size(4,1)
{
	iconSize = { 0.2f,0.2f };
	minimapIcon.SetTexture(TextureManager::Load("./Resources/minimap_icon.png", "minimapIcon"));
	minimapIcon.mMaterial.mColor = Color::kRed;
}

void EnemyUI::Update(Enemy* enemy)
{
	position = enemy->obj.mTransform.position;
	position.y += enemy->GetScale().y * 2;
	size.x = enemy->GetHP() / 4;
	maxSize.x = (enemy->GetMaxHP() / 4) * 1.1f;
	maxSize.y = size.y * 1.1f;

	//スクリーン座標を求める
	screenPos =
		Camera::sMinimapCamera->mViewProjection.WorldToScreen(
			enemy->obj.mTransform.position,
			50.f, static_cast<float>(RWindow::GetHeight()) - 200.f,
			150.f, 150.f, 0, 1);

	minimapIcon.mTransform.position = { screenPos.x,screenPos.y,0.f };
	//回転適用
	minimapIcon.mTransform.rotation.z =
		enemy->obj.mTransform.rotation.y;
	//決めたサイズに
	minimapIcon.mTransform.scale = { iconSize.x,iconSize.y,1.f };

	//更新忘れずに
	minimapIcon.mTransform.UpdateMatrix();
	minimapIcon.TransferBuffer();
}

void EnemyUI::Draw()
{
	Vector3 backPos = position;
	Vector3 targetEyeVec = Camera::sNowCamera->mViewProjection.mTarget - Camera::sNowCamera->mViewProjection.mEye;
	targetEyeVec.Normalize();
	backPos += targetEyeVec * 0.01f;	//ここをカメラ方向へのプラスに変えればok
	InstantDrawer::DrawGraph3D(backPos, maxSize.x, maxSize.y, "white2x2", Color::kBlack);
	InstantDrawer::DrawGraph3D(position, size.x, size.y, "white2x2", Color::kRed);

	//描画範囲
	RRect rect(
		50, 200,
		static_cast<long>(RWindow::GetHeight()) - 200,
		static_cast<long>(RWindow::GetHeight()) - 50);

	Renderer::SetScissorRects({ rect });
	minimapIcon.Draw();

	//元の範囲に戻してあげる
	RRect defaultRect(0, static_cast<long>(RWindow::GetWidth()), 0, static_cast<long>(RWindow::GetHeight()));
	Renderer::SetScissorRects({ defaultRect });
}