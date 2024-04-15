#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include "Wax.h"

class Enemy;

class WaxGroup
{
public:
	std::list<std::unique_ptr<Wax>> waxs;	//所属してるロウ

	Easing::EaseTimer solidTimer;	//固形になるまでの時間
	Easing::EaseTimer breakTimer;	//壊れるまでの時間
	
private:
	const uint32_t kMaxWax = 128;	//最大ロウ数

	//------------ HP関連 ------------//
	float hp;				//現在のヒットポイント
	float maxHP;			//最大HP
	float damageSustained;	//くらったダメージ
	bool isAlive;			//生存フラグ

public:
	WaxGroup();
	void Init();
	void Update();
	void Draw();
	void DrawUI();
	void DrawCollider();

	//ダメージ受ける
	void Damage(float damage) { damageSustained += damage; }

	//グループ内の蝋の固まるまでの時間を指定した時間に合わせる
	void SetSameSolidTime();

	// ゲッター //
	//生存フラグ取得
	bool GetIsAlive() { return isAlive; }
	//生存フラグ変更
	void SetIsAlive(bool isAlive_) { isAlive = isAlive_; }
	//今この瞬間にロウが固まったかを返す
	bool GetNowIsSolid();

	////満杯かどうか返す
	//bool GetIsFull() { return waxNums.size() >= kMaxWax; }
	////空っぽかどうか返す
	//bool GetIsEmpty() { return waxNums.empty(); }

	// セッター //
	//HP設定
	void SetHP(float hitPoint) { maxHP = hitPoint; hp = hitPoint; }
};