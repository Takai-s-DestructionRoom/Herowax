#pragma once
#include "BossState.h"
#include "GameObject.h"
#include <array>

class BossFallAttack : public BossState
{
public:
	BossFallAttack();
	void Update(Boss* boss)override;

public:
	bool isActive = false;

private:
	bool isStart;	//最初のフレームかフラグ

	float speed;	//落下速度
	float maxHight;	//最高高度

	int32_t partsNum;	//今回のパーツ数
};

