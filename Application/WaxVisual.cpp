#include "WaxVisual.h"
#include "WaxManager.h"
#include "Ground.h"

void WaxVisual::Init()
{
	//最初のずらし
	initInter.x = Util::GetRand(-10.f, 10.f);
	initInter.y = Util::GetRand(-10.f, 10.f);
	initInter.z = Util::GetRand(-10.f, 10.f);

	collider.pos = targetTrans.position + initInter;
	collider.r = 3;

	ground = Ground::GetInstance()->GetTransform();
}

void WaxVisual::Update()
{
	moveVec = { 0,0,0 };

	gravity += gravityAccel;
	moveVec.y -= Util::GetRand(gravity - gravity / 2, gravity + gravity / 2);

	moveVec.y *= 0.99f;

	//一旦張り付く
	if (isCling) {
		collider.pos = targetTrans.position + moveVec + initInter;
	}
	else
	{
		collider.pos += moveVec;
	}

	if (ground.position.y > collider.pos.y)
	{
		collider.pos.y = ground.position.y;
		gravity = 0.0f;

		isCling = false;
	}
}

void WaxVisual::Draw()
{

}

void WaxVisual::TransferBuffer()
{
	WaxManager::GetInstance()->slimeWax.spheres.emplace_back();
	WaxManager::GetInstance()->slimeWax.spheres.back().collider.pos = collider.pos;
	WaxManager::GetInstance()->slimeWax.spheres.back().collider.r = collider.r;
}

void WaxVisual::SetTarget(Transform target)
{
	targetTrans = target;
}

void WaxVisual::SetGround(Transform ground_)
{
	ground = ground_;
}
