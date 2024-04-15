#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Color.h"
#include "Sprite.h"

class Enemy;

class EnemyUI
{
public:
	//HP表示いったん意味ないのでコメントアウト
	//Vector3 position;
	//Vector2 size;
	//Vector2 maxSize;

	Vector2 screenPos;
	Vector2 iconSize;
	Sprite minimapIcon;
public:
	static void LoadResource();
	EnemyUI();
	void Update(Enemy* enemy);
	void Draw();
};
