#pragma once
#include "CircleGauge.h"

class Player;

class PlayerHpUI
{
public:
	CircleGauge circleGauge = {false};
	CircleGauge circleGaugeDam = { false };
	CircleGauge circleGaugeBack = { false };
	CircleGauge circleGaugeFrame = { false };

	void Init();
	void Update();
	void Draw();

	void SetPlayer(Player* player_);

private:
	float rate = 0.75f;

	float baseMin = 0.0f;
	float baseMax = 360.f;
	float baseRadian = 0.0f;
	float baseRadianDam = 0.0f;
	float oldRadian = 0.0f;

	Player* player = nullptr;
	float playerRangeY = 0.0f;
	float angleCompensNum = 85.f;
	bool checkBox = false;
};

