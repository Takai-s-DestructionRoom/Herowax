#include "PostEffectRenderStage.h"
#include <RDirectX.h>
#include <algorithm>

void PostEffectRenderStage::Init()
{
	mDefParamater.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	mDefParamater.renderTargets = { "RenderingImage" };
	mDefParamater.mRootSignature = RDirectX::GetDefRootSignature().mPtr.Get();
	mDefParamater.pipelineState = RDirectX::GetDefPipeline().mPtr.Get();
}

void PostEffectRenderStage::Render()
{
	struct sortObj {
		bool operator()(const RenderOrder& a, const RenderOrder& b) const noexcept {
			return a.anchorPoint.z < b.anchorPoint.z;
		}
	};
	std::stable_sort(mOrders.begin(), mOrders.end(), sortObj{});

	AllCall();
}
