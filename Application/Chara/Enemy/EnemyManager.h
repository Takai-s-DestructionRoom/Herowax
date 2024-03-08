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

	float speedMag;		//共通の移動速度倍率

public:
	static EnemyManager* GetInstance();

	//指定した座標に敵を生成
	void CreateEnemy(const Vector3 position);

	//敵の追跡対象を変更(プレイヤーを入れるのを想定)
	void SetTarget(ModelObj* target_);
	
	void Init();
	void Update();
	void Draw();

private:
	EnemyManager() {};
	~EnemyManager() {};

	//コピー禁止
	EnemyManager& operator=(const EnemyManager&) = delete;

	//敵の追跡対象(プレイヤーを入れるのを想定)
	ModelObj* target = nullptr;

	Easing::EaseTimer spawnTimer;
};

