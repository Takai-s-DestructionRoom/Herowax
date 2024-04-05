#pragma once
#include "Vector2.h"
#include "Color.h"
#include "Tower.h"
#include "Easing.h"
#include "Sprite.h"

class EggUI
{
private:
	Vector2 position = { 0,0 };
	Vector2 size = {0,0};
	Color color = { 0,0,0,0 };
	Color pinchColor = { 1,0,0,1 };
	Color warningColor = { 1,0.4f,0.4f,1 };

	Vector2 shake = { 0,0 };;
	float shakePower = 0;

	Tower* tower = nullptr;

	Easing::EaseTimer shakeTimer = 1.0f;
	int32_t oldHP = 0;

	Vector2 screenPos;
	Vector2 iconSize;
	Sprite minimapIcon;

public:
	static void LoadResource();

	EggUI();
	void Init();
	void Update();
	void Draw();

	void SetTower(Tower* tower);
};

