#pragma once
#include "LightObject.h"
#include <vector>

class BlinkSpotLightObject : public SpotLightObject
{
public:
	void Update();

private:
	//チカチカするタイミングをランダムで決定
	void SetBlinkTiming();

private:
	//点滅用のタイマー(装飾用の別クラスに分離した方がいいかも)
	Easing::EaseTimer blinkTimer = 5.0f;
	
	struct Timing 
	{
		Timing(float timing_, bool isCompleted_);

		bool Start(float targetTime);
		bool End();

		void Update();

	private:
		Easing::EaseTimer returnTimer = 0.1f;

		bool isCompleted = false;
		float timing = 0.0f;
	};

	//点滅タイミング
	std::vector<Timing> timings;
};

