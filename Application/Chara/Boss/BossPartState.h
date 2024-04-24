#pragma once
#include "Easing.h"
#include "Vector3.h"

class Parts;

class BossPartState
{
public:
	virtual void Update(Parts* part) = 0;
	virtual ~BossPartState() {};
};

class BossPartNormal : public BossPartState
{
public:
	void Update(Parts* part)override;
};

class BossPartCollect : public BossPartState
{
public:
	BossPartCollect();
	void Update(Parts* part)override;
private:
	
	Vector3 startPos;
	Vector3 startScale;
	Vector3 startRota;
	Vector3 endRota;

	bool isStart = true;
};

