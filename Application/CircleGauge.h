#pragma once
#include "Sprite.h"
#include "SRConstBuffer.h"
#include "BillboardImage.h"

struct Radial360Buffer
{
	float radian;
};

class CircleGauge
{
public:
	//0でsprite、1でbillboard
	CircleGauge(bool mode_ = 0);

	void Init(const std::string& title_ = "CircleGauge_Def");
	void Update();
	void Draw();

	void ImGui();

	void SetTexture(TextureHandle texture);
	void SetSize(Vector2 size);
	void SetColor(Color color);

private:
	RootSignature GetRootSignature() {
		return mRootSignature;
	}

	GraphicsPipeline GetGraphicsPipeline() {
		return mPipelineState;
	}
public:
	Transform baseTrans;

	float baseRadian = 0.0f;
	float rate = 1.0f;

	float angle = 0.0f;
	
private:
	BillboardImage billboard;
	Sprite sprite;

	SRConstBuffer<Radial360Buffer> mRadianBuffer;

	//0でsprite、1でbillboard
	bool mode = false;

	float nowRadian = 0.0f;
	float baseMax = 0.0f;
	float baseMin = 0.0f;

	std::string title = "";

	RootSignature mRootSignature;
	GraphicsPipeline mPipelineState;
};

