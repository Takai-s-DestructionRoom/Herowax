#include "Wax.h"
#include "Camera.h"

Wax::Wax():GameObject(),
	waxOriginColor(0.8f, 0.6f, 0.35f, 1.f),
	waxEndColor(0.8f,0.0f,0.f,1.f),
	igniteTimer(0.25f),
	burningTimer(1.0f),
	extinguishTimer(0.5f),
	state(new WaxNormal)
{
	obj = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));
	obj.mTuneMaterial.mColor = waxOriginColor;
}

void Wax::ChangeState(WaxState* newstate)
{
	delete state;
	state = newstate;
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

	//燃焼周りのステートの更新
	state->Update(this);

	UpdateCollider();

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Wax::Draw()
{
	obj.Draw();
}

bool Wax::IsBurning()
{
	return burningTimer.GetRun();
}

bool Wax::IsNormal() 
{
	return !igniteTimer.GetStarted() &&
		!burningTimer.GetStarted() && 
		!extinguishTimer.GetStarted();
}