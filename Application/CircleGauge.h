#pragma once
#include "Sprite.h"
#include "SRConstBuffer.h"

struct Radial360Buffer
{
	float radian;
};

class CircleGauge
{
public:
	void Init();
	void Update();
	void Draw();

private:
	RootSignature GetRootSignature() {
		return mRootSignature;
	}

	GraphicsPipeline GetGraphicsPipeline() {
		return mPipelineState;
	}
public:
	Sprite sprite;
	float radian = 0.0f;

private:
	SRConstBuffer<Radial360Buffer> mRadianBuffer;


	RootSignature mRootSignature;
	GraphicsPipeline mPipelineState;
};

