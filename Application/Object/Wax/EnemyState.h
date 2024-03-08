#pragma once

class Enemy;

class EnemyState
{
public:
	virtual void Update(Enemy* enemy) = 0;
	virtual ~EnemyState() {};
};

class EnemyNormal : public EnemyState
{
public:
	void Update(Enemy* enemy)override;
};

class EnemySlow : public EnemyState
{
public:
	void Update(Enemy* enemy)override;
};

class EnemyStop : public EnemyState
{
public:
	void Update(Enemy* enemy)override;
};

class EnemyBurning : public EnemyState
{
public:
	void Update(Enemy* enemy)override;
};