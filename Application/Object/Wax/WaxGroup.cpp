#include "WaxGroup.h"
#include "WaxManager.h"

WaxGroup::WaxGroup():
	hp(10.f),maxHP(10.f),damageSustained(0),isAlive(true)
{
}

void WaxGroup::Init()
{
	isAlive = true;
	hp = maxHP;
}

void WaxGroup::Update()
{
	hp = maxHP - damageSustained;

	//HPなくなったら死ぬ
	if (hp <= 0)
	{
		isAlive = false;
	}
}

void WaxGroup::Draw()
{
}