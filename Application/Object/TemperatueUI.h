#pragma once
#include "Vector2.h"
#include "Color.h"

//温度描画UI
class TemperatureUI
{
public:
	Vector2 position;
	Vector2 size;

	float angle = -60.f;

public:
	TemperatureUI();

	static void LoadResource();	//リソース読み込み

	void Update();
	void Draw();

private:
	void Save();
	Color color;

	Color coldColor = Color::kLightblue;	//冷えてる時の色
	Color hotColor = Color::kYellow;		//暖かい時の色
	Color burningColor = Color::kRed;		//めっちゃ熱い時の色

	float coldBorder;			//冷えてる色のボーダー
	float hotBorder;			//暖かい色のボーダー
	float burningBorder;		//めっちゃ熱い色のボーダー
};
