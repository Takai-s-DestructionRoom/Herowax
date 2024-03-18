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

	int32_t empCount = 0;
	//死んでいるロウがあれば消す
	for (auto itr = waxs.begin(); itr != waxs.end();)
	{
		//もし中身がないならカウントして次へ
		if (*itr == nullptr) {
			empCount++;
			itr++;
			continue;
		}
		if (!(*itr)->GetIsAlive())
		{
			//サイズ1ならバグることがあるので除外
			if (waxs.size() >= 1) {
				itr = waxs.erase(itr);
			}
			else {
				break;
			}
		}
		else
		{
			itr++;
		}
	}
	//もし中身が全部空なら中身をなくす
	if (empCount >= waxs.size()) {
		waxs.clear();
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
			wax->DrawCollider();
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
		if (!wax->isSolid) {
			//固まるまでの時間を更新
			wax->solidTimer.nowTime_ = smallestTime;
		}
	}
}

bool WaxGroup::IsSolid()
{
	int32_t solidCount = 0;
	//最大ロウ数より多い数が入ってたらだいたいアウトなのでfalse
	if (waxs.size() > kMaxWax) {
		return false;
	}

	//少なくともワックスを一つでも保持していて
	if (waxs.size() >= 1)
	{
		for (auto& wax : waxs)
		{
			if (wax->isSolid) {
				solidCount += 1;
			}
		}

		//すべてのロウが固まっているならtrue
		if (solidCount >= waxs.size()) {
			return true;
		}
	}
	//そうでないならfalse
	return false;
}
