#pragma once
#include "Vector2.h"
#include "Vector3.h"

class Player;

class PlayerUI
{
public:
	Vector3 position;
	Vector2 size;
	Vector2 maxSize;
public:
	static void LoadResource();
	PlayerUI();
	void Update(Player* player);
	void Draw();
};