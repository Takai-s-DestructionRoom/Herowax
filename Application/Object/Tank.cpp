#include "Tank.h"

Tank::Tank(ModelObj* target_) : Enemy(target_)
{
	obj = PaintableModelObj(Model::Load("./Resources/Model/enemy/enemy.obj", "enemy", true));
	//モデルを再設定するときはこのテクスチャももっかい設定する必要がある
	obj.mPaintDissolveMapTex = TextureManager::Load("./Resources/DissolveMap.png", "DissolveMapTex");

	std::string loadName = "./Resources/Data/StateOrder/Tank.txt";
	stateStrings = Util::GetFileData(loadName);

	shield.Init();
}

void Tank::Update()
{
	BaseUpdate();

	Vector3 frontVec = GetFrontVec().Normalize() * 5.f;
	frontVec.y = 0;
	shield.obj.mTransform.position = obj.mTransform.position + frontVec;
	shield.obj.mTransform.rotation = Quaternion::LookAt(frontVec).ToEuler();
	shield.obj.mTransform.rotation.y += Util::AngleToRadian(-90.f);

	shield.Update();
}

void Tank::Draw()
{
	Enemy::Draw();

	if (!GetIsSpawn()) return;

	if (isAlive)
	{
		shield.Draw();
	}
}

std::string Tank::GetEnemyTag()
{
	return "Tank";
}
