#pragma once
#include "ColPrimitive3D.h"
#include "Vector3.h"
#include "GameObject.h"
#include "ModelObj.h"

class WaxVisual
{
public:
	ModelObj obj;

	ColPrimitive3D::Sphere collider;
	Vector3 moveVec;

	void Init();
	void Update();
	void Draw();

	void TransferBuffer();

	void SetGround(Transform ground_);

	bool isAlive = true;

	int32_t power = 0;
	const int32_t THRESHOLD_POWER = 60;

private:
	Transform ground;

	//初期の位置ずらし
	Vector3 initInter;

	float gravity = 0.0f;
	float gravityAccel = 0.001f;

	//張り付きフラグ(一度でも地面についたら外れる)
	bool isCling = true;
};

