#pragma once
#include <stdint.h>

class Boss;

class BossState
{
public:
	virtual void Update(Boss* boss) = 0;
	virtual ~BossState() {};
	int32_t GetPriority() { return priority; }
public:
	int32_t priority = -1;	//優先度 
	//ステートが変化する際、変化先の優先度を参照し、
	//同じかより高い優先度でなければ変化できない
};

//通常時
class BossNormal : public BossState
{
public:
	BossNormal();
	void Update(Boss* boss)override;
};