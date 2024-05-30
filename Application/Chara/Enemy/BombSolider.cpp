#include "BombSolider.h"

BombSolider::BombSolider(ModelObj* target_) : Enemy(target_)
{
	obj = PaintableModelObj(Model::Load("./Resources/Model/dustBoxMan/dustBoxMan.obj", "dustBoxMan", true));
	//モデルを再設定するときはこのテクスチャももっかい設定する必要がある
	obj.mPaintDissolveMapTex = TextureManager::Load("./Resources/DissolveMap.png", "DissolveMapTex");

	std::string loadName = "./Resources/Data/StateOrder/BombSolider.txt";
	stateStrings = Util::GetFileData(loadName);

	SetModelSize({3.f,6.f,3.f});
}

void BombSolider::Update()
{
	BaseUpdate();
}

std::string BombSolider::GetEnemyTag()
{
	return "BombSolider";
}
