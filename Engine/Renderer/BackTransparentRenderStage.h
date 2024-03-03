#pragma once
#include "IRenderStage.h"
class BackTransparentRenderStage : public IRenderStage
{
public:
	virtual std::string GetTypeIndentifier() override {
		return "BackTransparent";
	};

	virtual void Init() override;
	virtual void Render() override;

private:
	GraphicsPipeline mPipelineState;
};

