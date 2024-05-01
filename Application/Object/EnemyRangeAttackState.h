#pragma once
#include "EnemyAttackState.h"

class EnemyRangePreState : public EnemyAttackState
{
public:
	EnemyRangePreState();
	void Update(Enemy* enemy);

	static std::string GetStateStr();

private:
	Easing::EaseTimer lifeTimer;
	Easing::EaseTimer blinkTimer;
};

class EnemyRangeNowState : public EnemyAttackState
{
public:
	EnemyRangeNowState();
	void Update(Enemy* enemy);

	static std::string GetStateStr();

private:
	Easing::EaseTimer lifeTimer;
	Easing::EaseTimer blinkTimer;
};
