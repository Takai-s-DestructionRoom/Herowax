#pragma once
#include "IRenderStage.h"
class PostEffectRenderStage : public IRenderStage
{
public:
	virtual std::string GetTypeIndentifier() override {
		return "PostEffect";
	};

	virtual void Init() override;
	virtual void Render() override;
};

