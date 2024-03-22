#pragma once
#include "GameObject.h"
#include "SimpleParticle.h"
#include "Easing.h"

//炎出すキャラ
class FireUnit : public GameObject
{
public:
	//------------ 炎関係 ------------//
	float fireGauge;			//炎ゲージ
	float maxFireGauge;			//炎ゲージ最大値
	uint32_t fireStock;			//炎ストック
	uint32_t maxFireStock;		//炎ストック最大値

public:
	FireUnit();

	void Init();
	void Update();
	void Draw();
};