#pragma once
#include "CircleGauge.h"

class BonusUI
{
public:
	void Init();
	void Update();
	void Draw();
	
	CircleGauge circleGauge;

private:
	Sprite bonusText;
	Sprite bonusGetText;
};

