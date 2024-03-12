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
	state(new WaxNormal),
	solidTimer(1.f)
{
	obj = ModelObj(Model::Load("./Resources/Model/wax/wax.obj", "wax", true));
	obj.mTuneMaterial.mColor = waxOriginColor;
}

void Wax::ChangeState(WaxState* newstate)
{
	delete state;
	state = newstate;
}

bool Wax::GetIsSolidNow()
{
	if (isSolid && isSolided == false)
	{
		return true;
	}
	return false;
}

void Wax::Init(uint32_t power, Vector3 vec,float speed,
	Vector2 range, float size, float atkTime, float solidTime)
{
	//hp = maxHP;
	atkPower = power;
	atkVec = vec * speed;
	atkRange = range;
	atkSize = size;
	atkTimer = atkTime;
	atkTimer.Start();
	solidTimer = solidTime;
	solidTimer.Start();
}

void Wax::Update()
{
	atkTimer.Update();
	solidTimer.Update();

	isSolided = isSolid;	//1F前の状態を保存

	//段々大きくなる
	atkSize = Easing::OutBack(atkTimer.GetTimeRate());
	Vector3 waxScale = { atkRange.x,1.f,atkRange.y };	//蝋の大きさ
	waxScale *= atkSize;
	SetScale(waxScale);									//大きさを反映

	//空中にいる時だけ
	if (isGround == false)
	{
		obj.mTransform.position += atkVec;	//飛んでいく

		//重力加算
		atkVec.y -= gravity;
	}

	//地面に埋ってたら
	if (obj.mTransform.position.y - obj.mTransform.scale.y < 0.f)
	{
		//地面に触れるとこまで移動
		obj.mTransform.position.y = 0.f;
		
		isGround = true;
	}

	//もう少しで固まりそうなら
	if (solidTimer.GetTimeRate() > 0.7f)
	{
		//もう1色は更新し続け
		obj.mTuneMaterial.mColor = Color::kWhite;

		//トグルでtrueになったら色変えることで点滅
		isFlashing = ((int)(solidTimer.GetTimeRate() * 1000.0f) % 100 > 50);
		if (isFlashing ^ obj.mTuneMaterial.mColor == Color::kWhite)
		{
			obj.mTuneMaterial.mColor = Color::kYellow;
		}
	}
	else
	{
		obj.mTuneMaterial.mColor = waxOriginColor;
	}

	//固まるまでの時間過ぎたら固まる
	if (solidTimer.GetEnd())
	{
		isSolid = true;
		obj.mTuneMaterial.mColor = Color::kLightblue;
	}

	//燃焼周りのステートの更新
	state->Update(this);

	////HPなくなったら死ぬ
	//if (hp <= 0)
	//{
	//	isAlive = false;
	//}

	UpdateCollider();

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Wax::Draw()
{
	if (isAlive)
	{
		obj.Draw();
		DrawCollider();
	}
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