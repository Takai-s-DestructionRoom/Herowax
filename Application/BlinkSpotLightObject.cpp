#include "BlinkSpotLightObject.h"
#include "Util.h"

void BlinkSpotLightObject::Update()
{
	if (!blinkTimer.GetRun())
	{
		SetBlinkTiming();
		blinkTimer.Start();
	}
	
	blinkTimer.Update();
	
	//チカチカするタイミングに合わせて、そのタイミングで一瞬消す
	for (auto& timing : timings)
	{
		if (timing.Start(blinkTimer.GetTimeRate())) 
		{
			isActive = false;
		}
		if (timing.End())
		{
			isActive = true;
		}
	}

	for (auto& timing : timings)
	{
		timing.Update();
	}

	SpotLightObject::Update();
}

void BlinkSpotLightObject::SetBlinkTiming()
{
	timings.clear();

	int32_t rand = 1;

	//ランダムな回数、チカチカするタイミングを決定
	for (int32_t i = 0; i < rand; i++)
	{
		timings.emplace_back(Util::GetRand(0.f, 1.f),false);
	}
}

BlinkSpotLightObject::Timing::Timing(float timing_, bool isCompleted_)
{
	timing = timing_;
	isCompleted = isCompleted_;
}

bool BlinkSpotLightObject::Timing::Start(float targetTime)
{
	if (timing <= targetTime && !isCompleted) {
		isCompleted = true;
		returnTimer.Start();
		return true;
	}
	else {
		return false;
	}
}

bool BlinkSpotLightObject::Timing::End()
{
	return returnTimer.GetNowEnd() && isCompleted;
}

void BlinkSpotLightObject::Timing::Update()
{
	returnTimer.Update();
}
