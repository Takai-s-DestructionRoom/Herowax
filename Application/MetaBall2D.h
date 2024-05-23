#pragma once
#include "Sprite.h"
#include "SRConstBuffer.h"
#include "Color.h"
#include "Float4.h"

struct Metaball2DBuffer
{
	Float4 color_;
	Float4 strokecolor_;
	float cutoff_;
	float stroke_;
};

class MetaBall2D
{
public:
	static MetaBall2D* GetInstance();

	void Init();
	void Update();
	void Draw();

	void TransfarBuffer();
	void Imgui();
	void CraeteMetaBall();

private:
	MetaBall2D();
	~MetaBall2D();

	RootSignature GetRootSignature() {
		return mRootSignature;
	}

	GraphicsPipeline GetGraphicsPipeline() {
		return mPipelineState;
	}

public:
	//こいつにボールを追加していく
	std::vector<Sprite> metaballs;

	Color color = Color::kGreen;
	Color strokecolor = Color::kWhite;
	float cutoff = 0.3f;
	float stroke = 0.7f;

private:
	bool renderTargetFlag = false;
	int32_t createNum = 100;
	Vector2 createSize = { 0.1f,0.1f};
	
	RootSignature mRootSignature;
	GraphicsPipeline mPipelineState;

	SRConstBuffer<Metaball2DBuffer> metaball2DBuffer; 
	SRVertexBuffer mVertBuff;
	SRIndexBuffer mIndexBuff;
};

