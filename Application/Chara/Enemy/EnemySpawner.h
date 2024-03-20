#pragma once
#include "GameObject.h"
#include "Easing.h"
#include "SpawnOrderData.h"

class EnemySpawner : public GameObject
{
public:
	//------------ HP関連 ------------//
	float hp;					//現在のヒットポイント
	float maxHP;				//最大HP
	float spawnInterval;		//敵の出現間隔
	int spawnNum;				//敵の同時出現数
	float spawnRandomPos;		//敵が出現する際の中心位置からのズレ

	std::string loadOrderFilename;	//参照するorderDataのハンドル

private:
	SpawnOrderData orderData;
	Easing::EaseTimer lifeTimer;

public:
	EnemySpawner();
	void Init()override;
	void Update()override;
	void Draw()override;

	//敵を出現
	void PopEnemy(const Vector3 position,const SpawnOrderOnce& order);

	//ダメージくらう
	void Damage(float damage) { hp -= damage; }

	//敵の出現順を読み込む
	void Load(const std::string fileName);
};

