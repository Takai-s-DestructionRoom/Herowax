#include "Renderer.h"
#include <Texture.h>
#include <RDirectX.h>
#include <RenderTarget.h>
#include <RWindow.h>

#include <BackSpriteRenderStage.h>
#include <OpaqueRenderStage.h>
#include <TransparentRenderStage.h>
#include <SpriteRenderStage.h>
#include <FinalRenderStage.h>
#include <Util.h>
#include <PostEffectRenderStage.h>
#include <BackTransparentRenderStage.h>

void Renderer::Execute()
{
	Renderer* instance = GetInstance();
	RenderTarget::SetToTexture("RenderingImage");
	RenderTarget::GetRenderTexture("RenderingImage")->ClearRenderTarget();
	RenderTarget::GetRenderTexture("RenderingImage")->ClearDepthStencil();
	for (auto itr = instance->mStages.begin(); itr != instance->mStages.end(); itr++) {
		IRenderStage* stage = itr->get();
		if (stage->mFlagEnabled) stage->Render();
		stage->mOrders.clear();
	}
}

void Renderer::DrawCall(std::string stageID, D3D12_VERTEX_BUFFER_VIEW* vertView, D3D12_INDEX_BUFFER_VIEW* indexView, uint32_t indexCount, const std::vector<RootData>& rootData, const Vector3& anchorPoint)
{
	RenderOrder order;
	order.anchorPoint = anchorPoint;
	order.vertView = vertView;
	order.indexView = indexView;
	order.indexCount = indexCount;
	order.rootData = rootData;
	Renderer::DrawCall(stageID, order);
}

void Renderer::DrawCall(std::string stageID, SRVertexBuffer& vertBuff, SRIndexBuffer& indexBuff, uint32_t indexCount, const std::vector<RootData>& rootData, const Vector3& anchorPoint)
{
	RenderOrder order;
	order.anchorPoint = anchorPoint;
	order.vertBuff = vertBuff;
	order.indexBuff = indexBuff;
	order.indexCount = indexCount;
	order.rootData = rootData;
	Renderer::DrawCall(stageID, order);
}

void Renderer::DrawCall(std::string stageID, RenderOrder order)
{
	Renderer* instance = GetInstance();

	if (stageID.empty()) {
#ifdef _DEBUG
		OutputDebugStringA("RKEngine WARNING: Renderer::DrawCall() called without an stageID.\n");
#endif
		return;
	}

	for (auto itr = instance->mStages.begin(); itr != instance->mStages.end(); itr++) {
		IRenderStage* stage = itr->get();
		if (stage->GetTypeIndentifier() == stageID) {
			//未設定の項目をレンダラーの設定で自動で補完する
			//ここでも未設定のままになった場合はレンダーステージに任せる
			if(order.renderTargets.empty()) order.renderTargets = instance->mRenderTargets;
			if(order.primitiveTopology == D3D_PRIMITIVE_TOPOLOGY_UNDEFINED) order.primitiveTopology = instance->mPrimitiveTopology;
			if(order.viewports.empty()) order.viewports = instance->mViewports;
			if(order.scissorRects.empty()) order.scissorRects = instance->mScissorRects;
			if(order.mRootSignature == nullptr) order.mRootSignature = instance->mRootSignature;
			if(order.pipelineState == nullptr) order.pipelineState = instance->mPipelineState;

			stage->mOrders.emplace_back(std::move(order));
			return;
		}
	}

#ifdef _DEBUG
	OutputDebugStringA(("RKEngine WARNING: Renderer::DrawCall() : RenderStage(" + stageID + ") is not found.\n").c_str());
#endif
}

void Renderer::RemoveRenderStage(std::string id)
{
	Renderer* instance = GetInstance();

	if (id.empty()) {
#ifdef _DEBUG
		OutputDebugStringA("RKEngine WARNING: Renderer::RemoveRenderStage() called without an stageID.\n");
#endif
		return;
	}

	for (auto itr = instance->mStages.begin(); itr != instance->mStages.end(); itr++) {
		IRenderStage* stage = itr->get();
		if (stage->GetTypeIndentifier() == id) {
			instance->mStages.erase(itr);
			return;
		}
	}

#ifdef _DEBUG
	OutputDebugStringA(("RKEngine WARNING: Renderer::RemoveRenderStage() : RenderStage(" + id + ") is not found.\n").c_str());
#endif
}

IRenderStage* Renderer::GetRenderStage(std::string id)
{
	Renderer* instance = GetInstance();

	if (id.empty()) {
#ifdef _DEBUG
		OutputDebugStringA("RKEngine WARNING: Renderer::GetRenderStage() called without an stageID.\n");
#endif
		return nullptr;
	}

	for (auto itr = instance->mStages.begin(); itr != instance->mStages.end(); itr++) {
		IRenderStage* stage = itr->get();
		if (stage->GetTypeIndentifier() == id) {
			return stage;
		}
	}
	return nullptr;
}

std::vector<std::string> Renderer::GetRenderStageIDs()
{
	std::vector<std::string> result;
	Renderer* instance = GetInstance();
	for (auto itr = instance->mStages.begin(); itr != instance->mStages.end(); itr++) {
		result.push_back((*itr)->GetTypeIndentifier());
	}
	return result;
}

void Renderer::SetAllParamaterToAuto()
{
	SetRenderTargetToAuto();
	SetRootSignatureToAuto();
	SetPipelineToAuto();

	Viewport viewport{};
	viewport.width = static_cast<float>(RWindow::GetWidth());
	viewport.height = static_cast<float>(RWindow::GetHeight());
	viewport.topleftX = 0;
	viewport.topleftY = 0;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	RRect scissorRect{};
	scissorRect.left = 0;
	scissorRect.right = scissorRect.left + RWindow::GetWidth();
	scissorRect.top = 0;
	scissorRect.bottom = scissorRect.top + RWindow::GetHeight();

	GetInstance()->mViewports.clear();
	GetInstance()->mViewports.push_back(viewport);
	GetInstance()->mScissorRects.clear();
	GetInstance()->mScissorRects.push_back(scissorRect);
}

void Renderer::Init()
{
	RenderTarget::CreateRenderTexture(RWindow::GetWidth(), RWindow::GetHeight(), { 0, 0, 0, 1 }, "RenderingImage");
	
	AddDefRenderStageBack<BackSpriteRenderStage>();
	AddDefRenderStageBack<BackTransparentRenderStage>();
	AddDefRenderStageBack<OpaqueRenderStage>();
	AddDefRenderStageBack<TransparentRenderStage>();
	AddDefRenderStageBack<PostEffectRenderStage>();
	AddDefRenderStageBack<SpriteRenderStage>();
	AddDefRenderStageBack<FinalRenderStage>();
}
