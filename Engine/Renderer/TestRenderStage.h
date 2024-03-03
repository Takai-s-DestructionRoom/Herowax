#pragma once
#include "IRenderStage.h"
class TestRenderStage : public IRenderStage
{
public:
	virtual std::string GetTypeIndentifier() override {
		return "TestRenderStage";
	};

	virtual void Init() {}
	virtual void Render();
};

