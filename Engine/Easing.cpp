#include "Easing.h"
#include "Util.h"
#include "TimeManager.h"

float Easing::lerp(float start, float end, float timeRate)
{
	float easeVal = timeRate;
	return start * (1.0f - easeVal) + end * timeRate;
};

float Easing::InQuad(float timeRate)
{
	return timeRate * timeRate;
};

float Easing::OutQuad(float timeRate)
{
	return 1 - (1 - timeRate) * (1 - timeRate);
};

float Easing::InQuad(float start, float end, float timeRate)
{
	float dif = end - start;
	dif *= InQuad(timeRate);
	return start + dif;
}
float Easing::OutQuad(float start, float end, float timeRate)
{
	float dif = end - start;
	dif *= OutQuad(timeRate);
	return start + dif;
}

float Easing::InOutQuad(float timeRate)
{
	return (float)(timeRate < 0.5f ? 2 * timeRate * timeRate : 1 - pow(-2 * timeRate + 2, 2) / 2);
}

float Easing::InOutQuad(float start, float end, float timeRate)
{
	float dif = end - start;
	dif *= InOutQuad(timeRate);
	return start + dif;
}

float Easing::OutBounce(float timeRate)
{
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (timeRate < 1 / d1) {
		return n1 * timeRate * timeRate;
	}
	else if (timeRate < 2 / d1) {
		return n1 * (timeRate -= 1.5f / d1) * timeRate + 0.75f;
	}
	else if (timeRate < 2.5f / d1) {
		return n1 * (timeRate -= 2.25f / d1) * timeRate + 0.9375f;
	}
	else {
		return n1 * (timeRate -= 2.625f / d1) * timeRate + 0.984375f;
	}
}

float Easing::OutBounce(float start, float end, float timeRate)
{
	float dif = end - start;
	dif *= OutBounce(timeRate);
	return start + dif;
}

float Easing::InBounce(float start, float end, float timeRate) {

	float dif = end - start;
	dif *= 1 - OutBounce(1 - timeRate);
	return start + dif;
}

float Easing::InOutBounce(float start, float end, float timeRate)
{
	float dif = end - start;
	if (timeRate < 0.5)
	{
		dif *= (1 - OutBounce(1 - 2 * timeRate) / 2);
	}
	else
	{
		dif *= (1 + OutBounce(2 * timeRate - 1) / 2);
	}
	return start + dif;
}

float Easing::InElastic(float timeRate)
{
	const float c4 = Util::PI2 / 3.f;

	return timeRate == 0
		? 0
		: timeRate == 1
		? 1
		: -powf(2, 10 * timeRate - 10) * sinf((timeRate * 10 - 10.75f) * c4);
}

float Easing::OutElastic(float timeRate)
{
	const float c4 = Util::PI2 / 3;

	return timeRate == 0
		? 0
		: timeRate == 1
		? 1
		: powf(2, -10 * timeRate) * sinf((timeRate * 10 - 0.75f) * c4) + 1;
}

float Easing::InElastic(float start, float end, float timeRate)
{
	float dif = end - start;
	dif *= InElastic(timeRate);
	return start + dif;
}

float Easing::OutElastic(float start, float end, float timeRate)
{
	float dif = end - start;
	dif *= OutElastic(timeRate);
	return start + dif;
}

float Easing::InBack(float timeRate)
{
	const float c1 = 1.70158f;
	const float c3 = c1 + 1;

	return c3 * timeRate * timeRate * timeRate - c1 * timeRate * timeRate;
}

float Easing::InBack(float start, float end, float timeRate)
{
	float dif = end - start;
	dif *= InBack(timeRate);
	return start + dif;
}

float Easing::OutBack(float timeRate)
{
	const float c1 = 1.70158f;
	const float c3 = c1 + 1;

	return 1 + c3 * powf(timeRate - 1, 3) + c1 * powf(timeRate - 1, 2);
}

float Easing::OutBack(float start, float end, float timeRate)
{
	float dif = end - start;
	dif *= OutBack(timeRate);
	return start + dif;
}

float Easing::InOutBack(float timeRate)
{
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;

	return timeRate < 0.5
		? (powf(2 * timeRate, 2) * ((c2 + 1) * 2 * timeRate - c2)) / 2
		: (powf(2 * timeRate - 2, 2) * ((c2 + 1) * (timeRate * 2 - 2) + c2) + 2) / 2;
}

float Easing::InOutBack(float start, float end, float timeRate)
{
	float dif = end - start;
	dif *= InOutBack(timeRate);
	return start + dif;
}

float Easing::GoAndBackInQuad(float start, float end, float timeRate)
{
	//半分までは上へ
	if (timeRate < 0.5f) {
		return Easing::InQuad(start, end, timeRate * 2.0f);
	}
	//それ以降は元に
	else {
		return Easing::InQuad(end,start,
			(timeRate - 0.5f) * 2.f);
	}
}

void Easing::EaseTimer::Reset()
{
	nowTime_ = 0.0f;
	end_ = false;
	reverseend_ = false;

	run_ = false;
	reverse_ = false;
}

void Easing::EaseTimer::Start()
{
	nowTime_ = 0.0f;
	end_ = false;
	reverseend_ = false;

	run_ = true;
	reverse_ = false;
}

void Easing::EaseTimer::ReverseStart()
{
	end_ = false;
	reverseend_ = false;

	run_ = false;
	reverse_ = true;
}

void Easing::EaseTimer::Stop()
{
	run_ = false;
}

void Easing::EaseTimer::ReStart()
{
	run_ = true;
}

void Easing::EaseTimer::Update(const float elapseTimer)
{
	//前フレームの経過時間を記録
	oldTime_ = nowTime_;

	if (run_) {
		nowTime_ += TimeManager::deltaTime * elapseTimer;
	}
	if (reverse_) {
		nowTime_ -= TimeManager::deltaTime * elapseTimer;
	}
	// 経過時間が経過しきったらフラグを折る
	if (GetTimeRate() >= 1.0f && run_) {
		run_ = false;
		end_ = true;
	}
	//中途半端なところで止まっちゃうけど、とりあえず0.1以下になったら止める
	if (GetTimeRate() <= 0.001f && reverse_) {
		nowTime_ = 0.0f;
		reverse_ = false;
		reverseend_ = true;
	}
}

void Easing::EaseTimer::Roop(const float elapseTimer)
{
	Update(elapseTimer);
	roopend_ = false;
	if (GetStarted() == false)
	{
		Start();
	}
	else if (GetEnd())
	{
		Reset();
		roopend_ = true;
	}
}

void Easing::EaseTimer::RoopReverse(const float elapseTimer)
{
	Update(elapseTimer);
	roopend_ = false;
	roopReverseEnd_ = false;
	if (GetStarted() == false && GetReverseStarted() == false)
	{
		Start();
	}
	else if (GetEnd())
	{
		ReverseStart();
		roopend_ = true;
	}
	else if (GetReverseEnd())
	{
		Reset();
		roopReverseEnd_ = true;
	}
}

float Easing::EaseTimer::GetTimeRate()const
{
	float timeRate = 0.0f;
	timeRate = Util::Clamp(timeRate, nowTime_ / maxTime_, 1.0f);
	return timeRate;
}

Vector3 InQuadVec3(const Vector3& start, const Vector3& end, float timerate)
{
	Vector3 result;
	result.x = Easing::InQuad(start.x, end.x, timerate);
	result.y = Easing::InQuad(start.y, end.y, timerate);
	result.z = Easing::InQuad(start.z, end.z, timerate);
	return result;
}

Vector2 InQuadVec2(const Vector2& start, const Vector2& end, float timerate)
{
	Vector2 result;
	result.x = Easing::InQuad(start.x, end.x, timerate);
	result.y = Easing::InQuad(start.y, end.y, timerate);
	return result;
}

Vector3 OutQuadVec3(const Vector3& start, const Vector3& end, float timerate)
{
	Vector3 result;
	result.x = Easing::OutQuad(start.x, end.x, timerate);
	result.y = Easing::OutQuad(start.y, end.y, timerate);
	result.z = Easing::OutQuad(start.z, end.z, timerate);
	return result;
}

Vector3 InBackVec3(const Vector3& start, const Vector3& end, float timerate)
{
	Vector3 result;
	result.x = Easing::InBack(start.x, end.x, timerate);
	result.y = Easing::InBack(start.y, end.y, timerate);
	result.z = Easing::InBack(start.z, end.z, timerate);
	return result;
}
