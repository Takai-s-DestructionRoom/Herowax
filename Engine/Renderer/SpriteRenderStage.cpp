#include "SpriteRenderStage.h"
#include <algorithm>
#include "Renderer.h"
#include "Sprite.h"
#include <RInput.h>

void SpriteRenderStage::Init()
{
	mDefParamater.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	mDefParamater.renderTargets = { "RenderingImage" };
	mDefParamater.mRootSignature = SpriteManager::GetInstance()->GetRootSignature().mPtr.Get();
	mDefParamater.pipelineState = SpriteManager::GetInstance()->GetGraphicsPipeline().mPtr.Get();
}

void SpriteRenderStage::Render()
{
	struct sortObj {
		bool operator()(const RenderOrder& a, const RenderOrder& b) const noexcept {
			return a.anchorPoint.z < b.anchorPoint.z;
		}
	};
	std::stable_sort(mOrders.begin(), mOrders.end(), sortObj{});

	AllCall();
}
