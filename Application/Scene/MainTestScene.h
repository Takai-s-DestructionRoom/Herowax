/*
* @file MainTestScene.h
* @brief 色んなテストをするための落書き帳的シーン
*/

#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"
#include "SRConstBuffer.h"

class MainTestScene : public IScene
{
public:
	MainTestScene();
	~MainTestScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	DebugCamera camera = DebugCamera({0, 0, -5});
	LightGroup light;

	ModelObj skydome;

	ModelObj testModel;

	Transform meltAxis;
	struct TestSetting {
		float factor;
		float falloff;
		float radius;
		float top;
		float bottom;
		float pad[3];
		Matrix4 matMeshToAxis;
		Matrix4 matAxisToMesh;
		bool clampAtBottom;
	};
	SRConstBuffer<TestSetting> meltBuff;
	bool useWireframe = false;
};
