#pragma once
#include "Vector2.h"
#include "Color.h"
#include "Tower.h"
#include "Easing.h"

class EggUI
{
private:
	Vector2 position;
	Vector2 size;
	Color color;
	Color pinchColor = { 1,0,0,1 };
	Color warningColor = { 1,0.4f,0.4f,1 };

	Vector2 shake;
	float shakePower;

	Tower* tower = nullptr;

	Easing::EaseTimer shakeTimer;
	int32_t oldHP;

public:
	static void LoadResource();

	void Init();
	void Update();
	void Draw();

	void SetTower(Tower* tower);
};

