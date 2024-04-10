#pragma once
#include "BossState.h"
#include <vector>

class BossPunch : public BossState
{
public:
	BossPunch();
	BossPunch(bool isLeft);
	void Update(Boss* boss)override;

private:
	std::vector<Vector3> splinePoints;	//殴り始めてから地面に落ちるまでの挙動
	bool isLeft_;						//左で攻撃するかフラグ
	bool isStart;						//最初のフレームかフラグ
	Vector3 handToTarget;				//こぶしから目標までのベクトル
	float punchRadianX;
	float punchRadianY;
};