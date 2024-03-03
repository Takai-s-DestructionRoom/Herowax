#include <RDirectX.h>
#include "BackTransparentRenderStage.h"

void BackTransparentRenderStage::Init()
{
	mPipelineState = RDirectX::GetDefPipeline();

	// ラスタライザの設定
	mPipelineState.mDesc.BlendState.AlphaToCoverageEnable = false;

	mPipelineState.mDesc.DepthStencilState.DepthEnable = true;
	mPipelineState.mDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	mPipelineState.mDesc.pRootSignature = RDirectX::GetDefRootSignature().mPtr.Get();

	mPipelineState.Create();

	mDefParamater.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	mDefParamater.renderTargets = { "RenderingImage" };
	mDefParamater.mRootSignature = RDirectX::GetDefRootSignature().mPtr.Get();
	mDefParamater.pipelineState = mPipelineState.mPtr.Get();
}

void BackTransparentRenderStage::Render()
{
	AllCall();
}
