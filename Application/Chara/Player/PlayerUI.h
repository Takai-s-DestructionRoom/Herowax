#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Color.h"
#include <array>

class Player;

enum class UIType
{
	waxGauge,
	fireGauge,

	max
};

class PlayerUI
{
public:
	std::array<Vector3, 2> position;
	std::array<Vector2, 2> size;
	std::array<Vector2, 2> maxSize;
	std::array<Color, 2> gaugeColor;
public:
	static void LoadResource();
	PlayerUI();
	void Update(Player* player);
	void Draw();
};