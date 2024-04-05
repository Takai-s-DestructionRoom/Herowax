#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Color.h"
#include "Sprite.h"

class Minimap
{
public:
	Vector2 pos;
	float size;
	Color frameColor;
	Color backColor;

	Sprite backSprite;	//ミニマップ背景

	RRect rect;			//描画範囲
	RRect defaultRect;	//もともとの描画範囲

	bool isPlayerDraw;
	bool isEnemyDraw;
	bool isBossDraw;
	bool isWaxDraw;

	Minimap();

	static Minimap* GetInstance();

	void Init();
	void Update();
	void Draw();

	Vector2 GetScreenPos(Vector3 wpos);
};