#include "EnemyUI.h"
#include "Enemy.h"
#include "InstantDrawer.h"
#include "Texture.h"
#include "Camera.h"

void EnemyUI::LoadResource()
{
	TextureManager::Load("./Resources/white2x2.png", "white2x2");
}

EnemyUI::EnemyUI() : position(1,1,1),size(4,1)
{
}

void EnemyUI::Update(Enemy* enemy)
{
	position = enemy->obj.mTransform.position;
	position.y += enemy->GetScale().y * 2;
	size.x = enemy->GetHP() / 4;
	maxSize.x = (enemy->GetMaxHP() / 4) * 1.1f;
	maxSize.y = size.y * 1.1f;
}

void EnemyUI::Draw()
{
	Vector3 backPos = position;
	Vector3 targetEyeVec = Camera::sNowCamera->mViewProjection.mTarget - Camera::sNowCamera->mViewProjection.mEye;
	targetEyeVec.Normalize();
	backPos += targetEyeVec * 0.01f;	//ここをカメラ方向へのプラスに変えればok
	InstantDrawer::DrawGraph3D(backPos, maxSize.x, maxSize.y, "white2x2", Color::kBlack);
	InstantDrawer::DrawGraph3D(position, size.x, size.y, "white2x2", Color::kRed);
}
