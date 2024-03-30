#pragma once
#include "IScene.h"
#include "Util.h"
#include <vector>
#include "ModelObj.h"
#include "DebugCamera.h"
#include "LightGroup.h"

class RayMarchTestScene : public IScene
{
public:
	~RayMarchTestScene() {}

	void Init();
	void Update();
	void Draw();
	void Finalize();

	GraphicsPipeline SlimeShaderPipeLine();

public:
	ModelObj plane; 
	DebugCamera camera = DebugCamera({ 0, 0, -5 });
	LightGroup light;
};

