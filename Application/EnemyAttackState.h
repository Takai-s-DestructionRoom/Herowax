#pragma once
#include "Easing.h"
#include "ModelObj.h"

class Enemy;

class EnemyAttackState
{
public:
	static void LoadResource();
	virtual void Update(Enemy* enemy) = 0;
	virtual ~EnemyAttackState() {};
};

class EnemyNonAttackState : public EnemyAttackState
{
public:
	void Update(Enemy* enemy);

};

class EnemyFindState : public EnemyAttackState
{
public:
	EnemyFindState();
	void Update(Enemy* enemy);
private:
	Easing::EaseTimer lifeTimer = 0.5f;
	Vector3 position;
	bool isStart = false;

	float startY;
	float endY;
};

class EnemyPreState : public EnemyAttackState
{
public:
	EnemyPreState();
	void Update(Enemy* enemy);
private:
	Easing::EaseTimer lifeTimer;
	Easing::EaseTimer brinkTimer;

	bool isStart = false;
	Vector3 start = { 0,0,0 };
	Vector3 end = { 0,0,0 };

};

class EnemyNowAttackState : public EnemyAttackState
{
public:
	EnemyNowAttackState();
	void Update(Enemy* enemy);
private:
	bool isStart = false;
	Easing::EaseTimer chargeTimer;	//突進タイマー
	float oldChargeTime = 0.0f;		//1フレ前の突進タイマー
	Easing::EaseTimer postureTimer;	//姿勢タイマー
};