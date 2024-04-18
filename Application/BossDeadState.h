#pragma once
#include "BossState.h"

class Boss;

class BossDeadState : public BossState
{
public:
	BossDeadState();
	void Update(Boss* boss)override;

private:
	//吸収用のパラメータ
	Easing::EaseTimer absorpTimer = 1.0f;
	Vector3 startPos;
	Vector3 startScale;
	Vector3 startRota;
	Vector3 endRota;

	bool isStart = true;

};

