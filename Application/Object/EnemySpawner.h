#pragma once
#include "GameObject.h"
#include "Easing.h"

class EnemySpawner : public GameObject
{
public:
	//------------ HP関連 ------------//
	int hp;						//現在のヒットポイント
	int maxHP;					//最大HP
	float spawnInterval;		//敵の出現間隔
	int spawnNum;				//敵の同時出現数
	float spawnRandomPos;		//敵が出現する際の中心位置からのズレ

private:
	Easing::EaseTimer spawnTimer;

public:
	EnemySpawner();
	void Init()override;
	void Update()override;
	void Draw()override;

	//敵を指定秒数感覚で出現
	void PopEnemy(const Vector3 position,float time);

	//ダメージくらう
	void Damage(uint32_t damage) { hp -= damage; }
};

