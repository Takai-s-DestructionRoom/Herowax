#pragma once

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
	void Update(Enemy* enemy)override;
};

//蝋をかけられて固まった状態(前進に蝋がついているイメージ)
class EnemyWaxStop : public EnemyState
{
public:
	void Update(Enemy* enemy)override;
};

//固まってしまった状態
class EnemyStop : public EnemyState
{
public:
	void Update(Enemy* enemy)override;
};

//燃えてる状態
class EnemyBurning : public EnemyState
{
public:
	void Update(Enemy* enemy)override;
};