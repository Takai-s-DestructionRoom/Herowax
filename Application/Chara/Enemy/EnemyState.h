#pragma once
#include "Easing.h"

class Enemy;

class EnemyState
{
public:
	virtual void Update(Enemy* enemy) = 0;
	virtual ~EnemyState() {};
};

//通常時
class EnemyNormal : public EnemyState
{
public:
	void Update(Enemy* enemy)override;
};

//蝋に足を取られて減速状態
class EnemySlow : public EnemyState
{
public:
	void Update(Enemy* enemy)override;
};

//蝋をかけられてる状態
class EnemyWaxCoating : public EnemyState
{
public:
	EnemyWaxCoating();
	void Update(Enemy* enemy)override;
private:
	Easing::EaseTimer timer = 1.0f;
};

//全身硬化状態(全身に蝋がついているイメージ)
class EnemyAllStop : public EnemyState
{
public:
	void Update(Enemy* enemy)override;
};

//足元硬化状態
class EnemyFootStop : public EnemyState
{
public:
	void Update(Enemy* enemy)override;
};

//燃えてる状態
class EnemyBurning : public EnemyState
{
public:
	EnemyBurning();
	void Update(Enemy* enemy)override;
private:
	Easing::EaseTimer timer = 1.0f;
};