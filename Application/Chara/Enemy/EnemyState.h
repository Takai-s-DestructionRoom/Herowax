#pragma once
#include "Easing.h"
#include "Color.h"

class Enemy;

class EnemyState
{
public:
	virtual void Update(Enemy* enemy) = 0;
	virtual ~EnemyState() {};
	int32_t GetPriority();
public:
	int32_t priority = -1;	//優先度 
	//ステートが変化する際、変化先の優先度を参照し、
	//同じかより高い優先度でなければ変化できない
};

//通常時
class EnemyNormal : public EnemyState
{
public:
	EnemyNormal();
	void Update(Enemy* enemy)override;
};

//蝋に足を取られて減速状態
class EnemySlow : public EnemyState
{
public:
	EnemySlow();
	void Update(Enemy* enemy)override;
};

//全身硬化状態(全身に蝋がついているイメージ)
class EnemyAllStop : public EnemyState
{
public:
	EnemyAllStop();
	void Update(Enemy* enemy)override;
private:
	Color saveColor;
	Enemy* saveEnemy = nullptr;
};

//燃えてる状態
class EnemyBurning : public EnemyState
{
public:
	EnemyBurning();
	void Update(Enemy* enemy)override;
private:
	Easing::EaseTimer timer = 1.0f;

	uint32_t fireAddFrame = 5;	//炎を何フレームに一回追加するか
	uint32_t frameCount;		//フレームカウント
};

class EnemyPreAttack : public EnemyState
{
public:

};