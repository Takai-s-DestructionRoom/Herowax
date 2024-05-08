#pragma once
#include "IScene.h"
#include <string>
#include "Easing.h"
#include "Vector3.h"
#include "DebugCamera.h"
#include "LightGroup.h"
#include "ModelObj.h"

class ParticleEditorScene : public IScene
{
public:
	ParticleEditorScene();
	~ParticleEditorScene() {}

	void Init()override;
	void Update()override;
	void Draw()override;
	void Finalize()override;

private:
	ModelObj skydome;	//天球
	ModelObj drawEmitter;
	Vector3 emitPos = { 0,0,0 };
	std::string loadPartName = "";
	Easing::EaseTimer roopTimer = 0.3f;
	bool isAutoCreate = true;
	bool camForce = true;

	DebugCamera camera = DebugCamera({ 0, 0, -5 });
	LightGroup light;
	std::vector<std::string> fileNames;
	//std::string fileNames;
};

