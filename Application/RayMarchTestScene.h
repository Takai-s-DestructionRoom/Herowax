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
#include "Vector3.h"
#include "WaxVisual.h"
#include "GameObject.h"

class DebugObject : public GameObject
{
public:
	void Init()override;
	void Update()override;
	void Draw()override;
};

class RayMarchTestScene : public IScene
{
public:
	~RayMarchTestScene() {}

	void Init();
	void Update();
	void Draw();
	void Finalize();

private:
	void CreateWaxVisual();

private:
	DebugObject debugObj;

	ModelObj skydome;
	ModelObj plane;
	
	DebugCamera camera = DebugCamera({ 0, 0, -5 });
	LightGroup light;

	//見た目用のロウ
	std::vector<std::unique_ptr<WaxVisual>> waxVisual;
	Easing::EaseTimer createTimer = 0.25f;

	bool autoView = false;
};

