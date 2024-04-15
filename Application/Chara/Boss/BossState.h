#pragma once
#include <stdint.h>
#include "Easing.h"

class Boss;

class BossState
{
protected:
	bool isFinished = false;	//行動終了したかフラグ

public:
	virtual void Update(Boss* boss) = 0;
	virtual ~BossState() {};

	// ゲッター //
	//行動終了したかフラグ取得
	bool GetIsFinished() { return isFinished; }
	//優先度取得
	int32_t GetPriority() { return priority; }

public:
	int32_t priority = -1;	//優先度 
	//ステートが変化する際、変化先の優先度を参照し、
	//同じかより高い優先度でなければ変化できない
};

