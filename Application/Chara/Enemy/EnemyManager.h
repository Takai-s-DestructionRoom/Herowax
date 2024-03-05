#pragma once
#include "ModelObj.h"
#include "Enemy.h"
#include <list>

class EnemyManager
{
public:
	EnemyManager();

	//指定した座標に敵を出現させる
	void PopEnemy(const Vector3 position);

	//敵の追跡対象を変更(プレイヤーを入れるのを想定)
	void SetTarget(ModelObj* target_);
	
	void Init();
	void Update();
	void Draw();

private:
	//敵の追跡対象(プレイヤーを入れるのを想定)
	ModelObj* target = nullptr;
	
	//敵リスト
	std::list<Enemy> enemys;
};

