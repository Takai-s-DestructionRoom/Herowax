#pragma once
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
};

class WaxIgnite : public WaxState
{
public:
	WaxIgnite();
	void Update(Wax* wax)override;
};

class WaxBurning : public WaxState
{
public:
	void Update(Wax* wax)override;
};

class WaxExtinguish : public WaxState
{
public:
	void Update(Wax* wax)override;
private:
	Vector3 saveScale = { 0,0,0 };
};