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
#include "Float4.h"
#include "RenderTarget.h"

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
	ModelObj testObj;

	struct TankData {
		Vector3 centerPos;
		float upper;
		float lower;
		float amplitude;
		float frequency;
		float time;
	};
	SRConstBuffer<TankData> tankBuff;

	RootSignature* GetRootSig();
	GraphicsPipeline* GetPipeline();
	GraphicsPipeline* GetPipelineB();
};
