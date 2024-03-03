#pragma once
#include "IRenderStage.h"
class BackSpriteRenderStage : public IRenderStage
{
public:
	virtual std::string GetTypeIndentifier() override {
		return "BackSprite";
	};

	virtual void Init() override;
	virtual void Render() override;
};

