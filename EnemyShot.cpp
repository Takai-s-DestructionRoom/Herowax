#include "EnemyShot.h"

void EnemyShot::LoadResource()
{ 
	//モデル読み込み
	Model::Load("./Resources/Model/hexagonBolt/hexagonBolt.obj", "hexagonBolt");
	Model::Load("./Resources/Model/gear/gear.obj", "gear");
	Model::Load("./Resources/Model/plusScrew/plusScrew.obj", "plusScrew");
}

void EnemyShot::Init()
{
	int32_t rand = Util::GetRand(0, 2);
	if (rand == 0) {
		obj = PaintableModelObj("hexagonBolt");
	}
	if (rand == 1) {
		obj = PaintableModelObj("gear");
	}
	if (rand == 2) {
		obj = PaintableModelObj("plusScrew");
	}

	obj.mTransform.scale = { 3,3,3 };
}

void EnemyShot::Update()
{
	//リセット
	moveVec = {0,0,0};

	rotVec = { 0,0,0 };

	//指定した場所までくるくるーって飛んでいく
	//座標加算
	obj.mTransform.position += moveVec;

	//回転の適用
	obj.mTransform.rotation = rotVec;

	BrightTransferBuffer(Camera::sNowCamera->mViewProjection);
}

void EnemyShot::Draw()
{
	BrightDraw();
}
