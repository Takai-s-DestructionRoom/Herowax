#pragma once
#include "LightObject.h"
#include <vector>
#include "ModelObj.h"

class BlinkSpotLightObject : public SpotLightObject
{
public:
	void Init()override;
	void Update()override;
	void Draw()override;

	void BlinkStart();

public:
	ModelObj stickObj;

private:
	//チカチカ用タイマー
	Easing::EaseTimer blinkRoopTimer = 0.05f;
	//チカチカクールタイム
	Easing::EaseTimer blinkCoolTimer = 5.0f;

	int32_t count = 0;
	const int32_t BLINK_COUNT = 3;

	const float COOLTIME_MIN = 5.0f;
	const float COOLTIME_MAX = 10.0f;

	Color saveColor = { -1,-1,-1,1};
};

