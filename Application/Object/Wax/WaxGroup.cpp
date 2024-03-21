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
	solidBreakTimer.Update();

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
			itr = waxs.erase(itr);
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
		wax->obj.mTuneMaterial.mColor = wax->SolidBling(solidTimer);
		wax->Update();
	}

	for (auto& enemy : trapEnemys)
	{
		if (enemy->GetState() == "Normal") {
			enemy->ChangeState<EnemySlow>();
		}
	}

	//1~9までの場合を入れる
	for (int i = 0; i < 10; i++)
	{
		if (trapEnemys.size() - 1 == i) {
			solidBreakTimer.maxTime_ = WaxManager::GetInstance()->waxTime[i];
			break;
		}	
	}
	//10以上の場合を入れる
	if (trapEnemys.size() - 1 >= 10) {
		solidBreakTimer.maxTime_ = WaxManager::GetInstance()->waxTime[9];
	}

	if (solidTimer.GetNowEnd()) {
		//捕まえてるエネミーを足止めする
		for (auto& enemy : trapEnemys)
		{
			enemy->ChangeState<EnemyFootStop>();
		}

		solidBreakTimer.Start();
	}

	//捕まえてるエネミーを通常の状態に戻す処理
	if (solidBreakTimer.GetEnd()) {
		for (auto& enemy : trapEnemys)
		{
			enemy->ChangeState<EnemyNormal>();
		}
		SetIsAlive(false);
	}

	//捕まえてるエネミーを燃やす処理
	for (auto& enemy : trapEnemys)
	{
		for (auto& wax : waxs)
		{
			bool isCollision = ColPrimitive3D::CheckSphereToSphere(enemy->collider,
				wax->collider);

			if (isCollision && wax->GetState() != "Normal") {
				enemy->ChangeState<EnemyBurning>();
			}
		}
	}

	if (!solidTimer.GetStarted()) {
		trapEnemys.clear();
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