#pragma once
#include "BossState.h"
#include <vector>
#include <array>

class BossAppearance : public BossState
{
public:
	BossAppearance();
	void Update(Boss* boss)override;

private:
	std::array<std::vector<Vector3>, 2> splinePoints;	//殴り始めてから地面に落ちるまでの挙動
	bool isStart;										//最初のフレームかフラグ
	std::array<Vector3, 2> handToTarget;				//こぶしから目標までのベクトル
	std::array<float,2> handRadianX;
	std::array<float,2> handRadianY;
	Easing::EaseTimer handEaseTimer = 0.8f;				//手のイージング制御用
};

