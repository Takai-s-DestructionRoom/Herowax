#include "WaxGroup.h"
#include "WaxManager.h"

WaxGroup::WaxGroup():hp(10.f),maxHP(10.f),damageSustained(0)
{
}

void WaxGroup::Init()
{
	hp = maxHP;
}

void WaxGroup::Update()
{
	hp = maxHP - damageSustained;

	//HPなくなったら死ぬ
	if (hp <= 0)
	{
		waxNums.clear();
	}
}

void WaxGroup::Draw()
{
}