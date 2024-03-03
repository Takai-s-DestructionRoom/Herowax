#pragma once
#include "IRenderStage.h"
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <RootSignature.h>
#include <GraphicsPipeline.h>

class FinalRenderStage : public IRenderStage
{
public:
	virtual std::string GetTypeIndentifier() override {
		return "Final";
	};

	virtual void Init() override;
	virtual void Render() override;

private:
	VertexBuffer mVertBuff;
	IndexBuffer mIndexBuff;

	RootSignature mRootSignature;
	GraphicsPipeline mPipelineState;
};

