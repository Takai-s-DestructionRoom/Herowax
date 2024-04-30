#include "BombSolider.h"

BombSolider::BombSolider(ModelObj* target_) : Enemy(target_)
{
	obj = PaintableModelObj(Model::Load("./Resources/Model/bombSolider/bombSolider.obj", "bombSolider", true));
	//モデルを再設定するときはこのテクスチャももっかい設定する必要がある
	obj.mPaintDissolveMapTex = TextureManager::Load("./Resources/DissolveMap.png", "DissolveMapTex");
}

void BombSolider::Update()
{
	BaseUpdate();
}
