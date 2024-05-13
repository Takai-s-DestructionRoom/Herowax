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
	float accel = 0.f;			//回収の加速度
	bool isStart = true;
	float oldTimeRate = 0.0f;
	Vector3 startPos;
};