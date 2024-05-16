#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Color.h"
#include "Sprite.h"
#include <array>
#include "CircleGauge.h"
#include "NumDrawer.h"
#include "PlayerHpUI.h"

class Player;

class PlayerUI
{
public:
	NumDrawer numDrawer;

	Vector2 screenPos;
	Vector2 iconSize;
	Sprite minimapIcon;

	Vector2 screenPosRange;
	Vector2 rangeSize;
	Sprite minimapIconRange;	//ミニマップに表示する回収範囲

private:
	PlayerHpUI playerHpUI;

	std::vector<CircleGauge> waxCircleGauges;	//最大値が増えたあとのロウゲージ
	std::vector<CircleGauge> waxCircleGaugeBacks;	//最大値が増えたあとのロウゲージ

	Vector3 basePos;
	Vector3 baseScale;

	float sizeCoeff = 1.0f;			//サイズの上昇係数
	float sizeCoeffPlus = 0.2f;			//サイズの上昇係数の上昇値
	float baseRadian = 0.0f;		//全体のロウの総量をゲージ事に0.0f~1.0fで管理
	float baseBackRadian = 0.0f;	//全体のロウの総量をゲージ事に0.0f~1.0fで管理
									//ゲージが2本になったら1.0f~2.0fになる
public:
	static void LoadResource();
	PlayerUI();
	void Update(Player* player);
	void Draw();

private:
	void GaugeReset();
	void GaugeAdd();
	void GaugeUpdate();
	void GaugeDraw();
};