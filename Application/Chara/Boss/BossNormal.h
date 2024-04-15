#pragma once
#include "BossState.h"
#include <array>

//通常時
class BossNormal : public BossState
{
public:
	BossNormal();
	void Update(Boss* boss)override;

private:
	Vector3 startObjRot;
	std::array<Vector3, 2> startPos, startRot;

	Easing::EaseTimer floatingTimer;		//ふよふよタイマー
	Easing::EaseTimer interpolationTimer;	//座標補間タイマー
};