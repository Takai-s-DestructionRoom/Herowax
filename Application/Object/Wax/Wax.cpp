#include "Wax.h"
#include "Camera.h"

Wax::Wax():GameObject(),
	waxOriginColor(0.8f, 0.6f, 0.35f, 1.f),
	waxEndColor(0.8f,0.0f,0.f,1.f),
	atkSpeed(1.f),
	atkPower(1),
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

void Wax::Init(uint32_t power, Vector3 vec,
	float speed, Vector2 range, float size,float time)
{
	atkPower = power;
	atkVec = vec * speed;
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
	/*SetPos(atkVec * atkRange.y * atkSpeed *
		Easing::OutBack(atkTimer.GetTimeRate()));*/

	//空中にいる時だけ
	if (isGround == false)
	{
		obj.mTransform.position += atkVec;

		//重力加算
		atkVec.y -= gravity;
	}

	//地面に埋ってたら
	if (obj.mTransform.position.y - obj.mTransform.scale.y < 0.f)
	{
		//地面に触れるとこまで移動
		obj.mTransform.position.y = 0.f + obj.mTransform.scale.y;
		
		isGround = true;
	}

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