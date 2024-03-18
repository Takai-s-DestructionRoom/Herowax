#pragma once
#include "Vector2.h"
#include "Vector3.h"

class Enemy;

class EnemyUI
{
public:
	Vector3 position;
	Vector2 size;
	Vector2 maxSize;
public:
	static void LoadResource();
	EnemyUI();
	void Update(Enemy* enemy);
	void Draw();
};
