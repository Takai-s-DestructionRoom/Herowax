#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include "Wax.h"

class WaxGroup
{
public:
	std::vector<std::unique_ptr<Wax>> waxs;	//所属してるロウ

	//所属しているロウの中で最大の時間
	float smallestTime = 1000.0f;

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
	//グループ内のロウが固まっているか返す
	bool IsSolid();

	////満杯かどうか返す
	//bool GetIsFull() { return waxNums.size() >= kMaxWax; }
	////空っぽかどうか返す
	//bool GetIsEmpty() { return waxNums.empty(); }

	// セッター //
	//HP設定
	void SetHP(float hitPoint) { maxHP = hitPoint; hp = hitPoint; }
};