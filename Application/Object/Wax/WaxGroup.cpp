#include "WaxGroup.h"
#include "WaxManager.h"
#include "Enemy.h"

WaxGroup::WaxGroup():
	hp(10.f),maxHP(10.f),damageSustained(0),isAlive(true)
{
	solidTimer.Start();
}

void WaxGroup::Init()
{
	isAlive = true;
	hp = maxHP;
}

void WaxGroup::Update()
{
	//もし管理しているロウがなくなったら死亡する
	if (waxs.size() <= 0) {
		SetIsAlive(false);
	}

	solidTimer.Update();
	breakTimer.Update();

	hp = maxHP - damageSustained;

	//HPなくなったら死ぬ
	if (hp <= 0 || breakTimer.GetEnd())
	{
		isAlive = false;
	}

	//死んでいるロウがあれば消す
	for (auto itr = waxs.begin(); itr != waxs.end();)
	{
		if (!(*itr)->GetIsAlive())
		{
			itr = waxs.erase(itr);
		}
		else
		{
			itr++;
		}
	}

	for (auto& wax : waxs)
	{
		wax->obj.mTuneMaterial.mColor = wax->SolidBling(solidTimer);
		wax->Update();
	}
}

void WaxGroup::Draw()
{
	for (auto& wax : waxs)
	{
		if (wax->isAlive) {
			wax->Draw();
		}
	}
}

void WaxGroup::DrawCollider()
{
	for (auto& wax : waxs)
	{
		if (wax->isAlive) {
			wax->DrawCollider();
		}
	}
}

void WaxGroup::SetSameSolidTime()
{
	//ロウをかけたらもう一度初めから開始
	solidTimer.Start();
}

bool WaxGroup::GetNowIsSolid()
{
	return solidTimer.GetNowEnd();
}