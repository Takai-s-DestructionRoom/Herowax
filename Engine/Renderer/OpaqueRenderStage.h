#pragma once
#include "IRenderStage.h"
class OpaqueRenderStage : public IRenderStage
{
public:
	virtual std::string GetTypeIndentifier() override {
		return "Opaque";
	};

	virtual void Init() override;
	virtual void Render() override;
};

