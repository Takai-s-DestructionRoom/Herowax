#pragma once
#include "IScene.h"
#include "Util.h"
#include <vector>
#include "ModelObj.h"
#include "DebugCamera.h"
#include "LightGroup.h"
#include "BillboardImage.h"
#include "ColPrimitive3D.h"
#include "Float4.h"
#include "Easing.h"
#include "SlimeWax.h"
#include "Vector3.h"

class RayMarchTestScene : public IScene
{
public:
	~RayMarchTestScene() {}

	void Init();
	void Update();
	void Draw();
	void Finalize();

public:

private:
	SlimeWax slimeWax;

	std::vector<Vector3> spline;
	Easing::EaseTimer timer = 1.0f;

	ModelObj skydome;
	ModelObj plane;
	
	ModelObj wasdKeyObj;
	ModelObj arrowKeyObj;

	DebugCamera camera = DebugCamera({ 0, 0, -5 });
	LightGroup light;

	float oldTime = 0;
	float delay1 = 0.1f;
	float delay2 = 0.2f;

	bool autoView = false;
};

