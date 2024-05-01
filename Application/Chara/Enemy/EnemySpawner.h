#pragma once
#include "GameObject.h"
#include "Easing.h"
#include "SpawnOrderData.h"
#include "EnemyBehaviorEditor.h"

class EnemySpawner : public GameObject
{
public:
	//------------ HP関連 ------------//
	float hp;					//現在のヒットポイント
	float maxHP;				//最大HP
	
	int spawnNum;				//敵の同時出現数
	float spawnRandomPos;		//敵が出現する際の中心位置からのズレ

	std::string loadOrderFilename;	//参照するorderDataのハンドル
	Easing::EaseTimer lifeTimer;

	float startTiming;	//スポナーが稼働開始するタイミング

private:
	SpawnOrderData orderData;
	std::string behaviorOrder;

public:
	EnemySpawner();
	void Init()override;
	void Init(const std::string& loadfile);
	void Update()override;
	void Draw()override;

	void LoadBehavior(const std::string& loadOrder);

	//このスポナーが敵を出現させているか(待機状態でないか)
	bool GetStarted();

	//タイマーのカウント開始
	void Start();

	//敵を出現
	void PopEnemy(const Vector3 position,const SpawnOrderOnce& order);

	//ダメージくらう
	void Damage(float damage) { hp -= damage; }

	//敵の出現順を読み込む
	void Load(const std::string fileName);
};

