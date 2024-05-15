#pragma once
#include "CircleGauge.h"

class Player;

class PlayerHpUI
{
public:
	CircleGauge circleGauge = {true};
	CircleGauge circleGaugeBack = {true};

	void Init();
	void Update();
	void Draw();

	void SetPlayer(Player* player_);

private:
	float rate = 0.75f;

	float baseMin = 0.0f;
	float baseMax = 360.f;
	float baseRadian = 0.0f;

	Player* player = nullptr;
	float playerRangeXZ = 3.0f;
	float playerRangeY = 0.0f;
	bool checkBox = false;
};

