#pragma once
#include "ColPrimitive3D.h"
#include "Vector3.h"
#include "GameObject.h"

class WaxVisual
{
public:
	ColPrimitive3D::Sphere collider;
	Vector3 inter;	//中心位置からどの位置に配置されるか

	void Init();
	void Update();
	void Draw();

	void SetTarget(Transform target);
	void SetGround(Transform ground_);

private:
	Transform targetTrans;
	Transform ground;

	float gravity = 0.0f;
	float gravityAccel = 0.001f;
};

