#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Color.h"
#include "Sprite.h"

class Boss;

class BossUI
{
public:
	Vector2 screenPos;
	Vector2 iconSize;
	Sprite minimapIcon;

public:
	static void LoadResource();
	BossUI();
	void Update(Boss* boss);
	void Draw();
};