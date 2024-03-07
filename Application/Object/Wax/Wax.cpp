#include "Wax.h"
#include "Camera.h"

Wax::Wax():GameObject()
{
	obj = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));
	obj.mTuneMaterial.mColor = { 0.8f,0.1f,0.1f,1.f };
}

void Wax::Init(uint32_t power, Vector3 vec, Vector3 originPos,
	float dist, Vector2 range, float size,float time)
{
	atkPower = power;
	atkVec = vec;
	atkOriginPos = originPos;
	atkDist = dist;
	atkRange = range;
	atkSize = size;
	atkTimer = time;
	atkTimer.Start();
}

void Wax::Update()
{
	atkTimer.Update();
	
	//段々大きくなる
	atkSize = Easing::OutBack(atkTimer.GetTimeRate());
	Vector3 waxScale = { atkRange.x,1.f,atkRange.y };	//蝋の大きさ
	waxScale *= atkSize;
	SetScale(waxScale);									//大きさを反映

	//出現位置と方向をもとに射出距離もかけて攻撃飛ばす
	SetPos(atkOriginPos + atkVec * atkRange.y * atkDist *
		Easing::OutBack(atkTimer.GetTimeRate()));

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Wax::Draw()
{
	obj.Draw();
}
