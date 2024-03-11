#pragma once
#include "ModelObj.h"
#include "Enemy.h"
#include <list>
#include "Easing.h"

class EnemyManager
{
public:
	//敵リスト
	std::list<Enemy> enemys;

private:
	uint32_t solidCombo;	//連続で固まった敵の数カウント(これが多いと抜け出すまでの時間減る)
	uint32_t burningCombo;	//連続で燃えた敵の数カウント(これが多いと温度爆上げ↑)

	Easing::EaseTimer burningComboTimer;	//燃焼コンボの繋がる猶予

	float slowMag;			//共通の減速率
	float slowCoatingMag;	//共通の蝋かけられたときの減速率

public:
	static EnemyManager* GetInstance();

	//指定した座標に敵を生成
	void CreateEnemy(const Vector3 position);

	//敵の追跡対象を変更(プレイヤーを入れるのを想定)
	void SetTarget(ModelObj* target_);
	
	void Init();
	void Update();
	void Draw();

	// ゲッター //
	//連続で固まった敵の数カウント取得
	uint32_t GetSolidCombo() { return solidCombo; }
	//連続で燃えた敵の数カウント取得
	uint32_t GetBurningCombo() { return burningCombo; }

	// セッター //
	//連続で固まった敵の数カウント増やす
	void IncrementSolidCombo() { solidCombo++; }
	//連続で燃えた敵の数カウント増やす
	void IncrementBurningCombo() { burningCombo++; burningComboTimer.Start(); }

private:
	EnemyManager() {};
	~EnemyManager() {};

	//コピー禁止
	EnemyManager& operator=(const EnemyManager&) = delete;

	//敵の追跡対象(プレイヤーを入れるのを想定)
	ModelObj* target = nullptr;

	Easing::EaseTimer spawnTimer;
};

