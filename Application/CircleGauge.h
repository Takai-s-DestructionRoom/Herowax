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

	void Init();
	void Update();
	void Draw();

	void SetTexture(TextureHandle texture);
	void SetBillboardSize(Vector2 size);
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

	float radian = 0.0f;

	float angle = 0.0f;

private:
	BillboardImage billboard;
	Sprite sprite;

	SRConstBuffer<Radial360Buffer> mRadianBuffer;

	//0でsprite、1でbillboard
	bool mode = false;

	RootSignature mRootSignature;
	GraphicsPipeline mPipelineState;
};

