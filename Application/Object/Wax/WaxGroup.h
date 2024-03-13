#pragma once
#include <cstdint>
#include <vector>

class WaxGroup
{
public:
	std::vector<uint32_t> waxNums;	//所属してるロウの要素番号

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

	//ダメージ受ける
	void Damage(float damage) { damageSustained += damage; }

	// ゲッター //
	//生存フラグ取得
	bool GetIsAlive() { return isAlive; }
	//満杯かどうか返す
	bool GetIsFull() { return waxNums.size() >= kMaxWax; }
	//空っぽかどうか返す
	bool GetIsEmpty() { return waxNums.empty(); }

	// セッター //
	//HP設定
	void SetHP(float hitPoint) { maxHP = hitPoint; hp = hitPoint; }
};