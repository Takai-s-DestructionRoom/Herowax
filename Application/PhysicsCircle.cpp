#include "PhysicsCircle.h"
#include "TimeManager.h"
#include "InstantDrawer.h"

void PhysicsCircle::addImpulseLocal(const Vector2& impulse, const Vector2& addLocalPos)
{
	v += impulse / M;
	omega += addLocalPos.Cross(impulse) / I;
}

void PhysicsCircle::addImpulse(const Vector2& impulse, const Vector2& addPos)
{
	addImpulseLocal(impulse, addPos - pos);
}

void PhysicsCircle::LoadResource()
{
	TextureManager::Load("./Resources/circle.png", "circle");
}

void PhysicsCircle::Update()
{
	pos += v * TimeManager::deltaTime;
	theta += omega * TimeManager::deltaTime;

	sphere.pos = pos;
	sphere.r = r;
}

void PhysicsCircle::Draw()
{
	InstantDrawer::DrawGraph(pos.x, pos.y,r,r,0,"circle");
}
