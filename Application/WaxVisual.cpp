#include "WaxVisual.h"
#include "WaxManager.h"
#include "Ground.h"

void WaxVisual::Init()
{
	inter.x = Util::GetRand(-10.f, 10.f);
	inter.y = Util::GetRand(-10.f, 10.f);
	inter.z = Util::GetRand(-10.f, 10.f);

	collider.pos = inter;
	collider.r = 3;

	ground = Ground::GetInstance()->GetTransform();
}

void WaxVisual::Update()
{
	gravity += gravityAccel;
	inter.y -= Util::GetRand(gravity - gravity / 2, gravity + gravity / 2);

	inter.y *= 0.99f;

	collider.pos = targetTrans.position + inter;

	if (ground.position.y > collider.pos.y)
	{
		collider.pos.y = ground.position.y;
		gravity = 0.0f;
	}

	WaxManager::GetInstance()->slimeWax.spheres.emplace_back();
	WaxManager::GetInstance()->slimeWax.spheres.back().collider.pos = collider.pos;
	WaxManager::GetInstance()->slimeWax.spheres.back().collider.r = collider.r;
}

void WaxVisual::Draw()
{

}

void WaxVisual::SetTarget(Transform target)
{
	targetTrans = target;
}

void WaxVisual::SetGround(Transform ground_)
{
	ground = ground_;
}
