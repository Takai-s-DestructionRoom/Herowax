#include "Charactor.h"

Charactor::Charactor() :
	moveSpeed(1.f), isGraund(true), hp(0), maxHP(10), isAlive(true)
{
}

void Charactor::UpdateCollider()
{
	collider.pos = GetPos();
	collider.r = 1;
}
