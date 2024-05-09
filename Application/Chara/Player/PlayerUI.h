#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Color.h"
#include "Sprite.h"
#include <array>
#include "CircleGauge.h"

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

	Vector2 screenPos;
	Vector2 iconSize;
	Sprite minimapIcon;

	Vector2 screenPosRange;
	Vector2 rangeSize;
	Sprite minimapIconRange;	//ミニマップに表示する回収範囲

private:
	CircleGauge waxCircleGauge;	//ロウの量を示すゲージ(円形)
	CircleGauge waxCircleGaugeBack;	//ロウの量を示すゲージ(円形)

	Vector3 basePos;
	Vector3 baseScale;

public:
	static void LoadResource();
	PlayerUI();
	void Update(Player* player);
	void Draw();
};