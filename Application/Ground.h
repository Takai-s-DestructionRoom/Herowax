#pragma once
#include "GameObject.h"
#include "Vector2.h"
#include "SRConstBuffer.h"

struct TilingBuffer
{
	Vector2 tiling;
};

class Ground
{
public:
	static Ground* GetInstance();

	void Init();
	void Update();
	void Draw();

	void SetStatus(const Vector3& pos, const Vector3& size, const Vector3& rot);

	Transform GetTransform();

private:
	ModelObj obj;

	Vector2 tiling = { 100,100 };

	SRConstBuffer<TilingBuffer> tilingBuffer;

	RootSignature mRootSignature;
	GraphicsPipeline mPipelineState;

	RootSignature GetRootSignature() {
		return mRootSignature;
	}

	GraphicsPipeline GetGraphicsPipeline() {
		return mPipelineState;
	}

	Ground() {};
	~Ground() {};
};