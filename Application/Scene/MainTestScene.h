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

struct WaxData {
	Float4 spheres[1024] = {};
	uint32_t sphereNum = 0;
	uint32_t rayMatchNum = 16;
	float hitThreshold = 0.001f;
	float smoothFactor = 4;
};

struct CameraData {
	Vector3 pos;
	float pad;
	Vector3 dir;
	float pad2;
	Vector3 up;
	float pad3;
	Matrix4 matViewProj;
	Matrix4 matInvView;
	Matrix4 matInvProj;
	Matrix4 matInvViewport;
};

struct HogeData {
	float hoge = 0.01f;
	float hoge2 = 0.01f;
};

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
	Sprite sprite;

	RenderTexture* rtTex;
	std::vector<Float4> spheres;
	int32_t sphereCreateNum = 10;

	Material material;
	SRVertexBuffer vertBuff;
	SRIndexBuffer indexBuff;
	SRConstBuffer<MaterialBuffer> materialBuff;
	SRConstBuffer<CameraData> cameraBuff;
	SRConstBuffer<WaxData> constBuff;
	SRConstBuffer<HogeData> hogeBuff;

	RootSignature* GetRootSig();
	GraphicsPipeline* GetPipeline();

	RootSignature* GetRootSigB();
	GraphicsPipeline* GetPipelineB();
};
