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
	//もし管理しているロウがなくなったら死亡する
	if (waxs.size() <= 0) {
		SetIsAlive(false);
	}

	hp = maxHP - damageSustained;

	//HPなくなったら死ぬ
	if (hp <= 0)
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
	for (auto& wax : waxs)
	{
		//より小さい時間を見つけたら記録
		if (smallestTime > wax->solidTimer.nowTime_)
		{
			smallestTime = wax->solidTimer.nowTime_;
		}
	}
	for (auto& wax : waxs)
	{
		//固まり初めていないなら
		if (wax->GetIsSolidLine()) {
			//固まるまでの時間を更新
			wax->solidTimer.nowTime_ = smallestTime;
		}
	}
}