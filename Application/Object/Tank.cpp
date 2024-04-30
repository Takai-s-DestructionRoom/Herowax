#include "Tank.h"

Tank::Tank(ModelObj* target_) : Enemy(target_)
{
	obj = PaintableModelObj(Model::Load("./Resources/Model/enemy/enemy.obj", "enemy", true));
	//モデルを再設定するときはこのテクスチャももっかい設定する必要がある
	obj.mPaintDissolveMapTex = TextureManager::Load("./Resources/DissolveMap.png", "DissolveMapTex");

	std::string loadName = "./Resources/Data/StateOrder/Tank.txt";
	stateStrings = Util::GetFileData(loadName);
}

void Tank::Update()
{
	BaseUpdate();
}
