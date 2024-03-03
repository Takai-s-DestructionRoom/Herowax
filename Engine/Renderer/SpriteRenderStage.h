#pragma once
#include "IRenderStage.h"
class SpriteRenderStage : public IRenderStage
{
public:
	virtual std::string GetTypeIndentifier() override {
		return "Sprite";
	};

	virtual void Init() override;
	virtual void Render() override;
};

