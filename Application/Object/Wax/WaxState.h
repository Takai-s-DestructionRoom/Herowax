#pragma once
#include <stdint.h>
#include <string>
#include "Vector3.h"
#include "Easing.h"

class Wax;

class WaxState
{
public:
	virtual void Update(Wax* wax) = 0;
	virtual ~WaxState() {};
};

class WaxNormal : public WaxState
{
public:
	void Update(Wax* wax)override;
	static std::string GetStateStr();
};

class WaxCollect : public WaxState
{
public:
	WaxCollect();
	void Update(Wax* wax)override;
	static std::string GetStateStr();

private:
	Easing::EaseTimer timer = 0.1f;
	float accel = 1.f;			//回収の加速度
	bool isStart = true;
	Vector3 startPos;

	float collectSpeed = 1.0f;
};