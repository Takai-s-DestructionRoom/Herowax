#pragma once
#include "IRenderStage.h"
class TransparentRenderStage : public IRenderStage
{
public:
	virtual std::string GetTypeIndentifier() override {
		return "Transparent";
	};

	virtual void Init() override;
	virtual void Render() override;

private:
	GraphicsPipeline mPipelineState;
};

