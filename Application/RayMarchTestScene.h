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

	DebugCamera camera = DebugCamera({ 0, 0, -5 });
	LightGroup light;
};

