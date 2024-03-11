#pragma once
#include "GameObject.h"
#include <vector>
#include "Vector3.h"
#include "Easing.h"

class Fire : public GameObject
{
public:
	Easing::EaseTimer timer;			//投げ始めてから地面に落ちるまでの時間
	std::vector<Vector3> splinePoints;	//投げ始めてから地面に落ちるまでの挙動(定義はFireManagerで行う)

	ModelObj targetCircle;

public:
	//生成時にFireManagerが定義していた挙動を自身に入れる
	Fire(std::vector<Vector3> splinePoints_);
	void Init()override;
	void Update()override;
	void Draw()override;
};

