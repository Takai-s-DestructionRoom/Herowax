#pragma once
#include "IScene.h"
#include "Camera.h"
#include "Vector2.h"
#include "LightGroup.h"
#include "ModelObj.h"
#include "DebugCamera.h"

class DeadScene : public IScene
{
public:
	DeadScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	DebugCamera camera = DebugCamera({ 0, 0, -5 });

	//Camera camera = Camera();
	Vector2 mAngle = { 0, 0 };
	LightGroup light;

	ModelObj obj; 
	ModelObj skydome;	//天球

	int32_t lightindex = 0;
	Vector3 atten;
	Color color;
	Vector3 dir;
	Vector2 factorAngle;
	Vector3 pos;
};

