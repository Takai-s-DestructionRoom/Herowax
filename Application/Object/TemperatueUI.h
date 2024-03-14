#pragma once
#include "Vector2.h"
#include "Color.h"

//温度描画UI
class TemperatureUI
{
public:
	Vector2 position;	//始点位置
	Vector2 size;

	Vector2 frameSize;		//枠の大きさ

	float angle = -60.f;

public:
	TemperatureUI();

	static void LoadResource();	//リソース読み込み

	void Update();
	void Draw();

private:
	void Save();

	Vector2 coldPos;
	Vector2 hotPos;
	Vector2 burningPos;

	Color frameColor = Color::kBlack;

	Color coldColor = Color::kLightblue;	//冷えてる時の色
	Color hotColor = Color::kYellow;		//暖かい時の色
	Color burningColor = Color::kRed;		//めっちゃ熱い時の色
};
