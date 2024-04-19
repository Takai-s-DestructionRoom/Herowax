#pragma once
#include "IScene.h"
#include "Vector3.h"
#include <vector>
#include "ModelObj.h"
#include "DebugCamera.h"
#include "LightGroup.h"
#include "Easing.h"
#include "EnemyBehaviorEditor.h"

class EasingCube
{
public:
	EasingCube();
	void Update();
	void Draw();

	ModelObj* start = nullptr;
	ModelObj* end = nullptr;

private:
	ModelObj obj;

	Easing::EaseTimer timer = 1.0f;
};

class BehaviorTestScene : public IScene
{
public:
	~BehaviorTestScene() {}

	void Init()override;
	void Update()override;
	void Draw()override;
	void Finalize()override;

private:
	void CubeCreate(Vector3 pos);

private:
	BehaviorData data;
	
	std::vector<ModelObj> objs;
	std::vector<EasingCube> lineCube;
	DebugCamera camera = DebugCamera({ 0, 0, -5 });
	LightGroup light;

	ModelObj skydome;

	std::string fileName = "";
};

