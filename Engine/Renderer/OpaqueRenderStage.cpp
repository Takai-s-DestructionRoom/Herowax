#include "OpaqueRenderStage.h"
#include <RDirectX.h>

void OpaqueRenderStage::Init()
{
	mDefParamater.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	mDefParamater.renderTargets = { "RenderingImage" };
	mDefParamater.mRootSignature = RDirectX::GetDefRootSignature().mPtr.Get();
	mDefParamater.pipelineState = RDirectX::GetDefPipeline().mPtr.Get();
}

void OpaqueRenderStage::Render()
{
	AllCall();
}
