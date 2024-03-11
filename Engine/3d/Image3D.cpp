#include "Image3D.h"
#include "RDirectX.h"
#include "Vertex.h"
#include "LightGroup.h"
#include <Renderer.h>

void Image3D::UpdateVertex()
{
	Vector2 texSize = {
		static_cast<float>(TextureManager::Get(mTexture).mResource->GetDesc().Width),
		static_cast<float>(TextureManager::Get(mTexture).mResource->GetDesc().Height)
	};

	float uvLeft = mSrcPos.x / texSize.x;
	float uvRight = (mSrcPos.x + mTexRect.x) / texSize.x;
	float uvTop = mSrcPos.y / texSize.y;
	float uvBottom = (mSrcPos.y + mTexRect.y) / texSize.y;

	//頂点データ
	VertexPNU vertices[] = {
		{{ -mAnchor.x * mSize.x, (1 - mAnchor.y) * mSize.y, 0.0f}, {0, 0, -1}, {uvLeft, uvTop}}, //左上
		{{ -mAnchor.x * mSize.x, -mAnchor.y * mSize.y, 0.0f }, {0, 0, -1}, {uvLeft, uvBottom}}, //左下
		{{ (1 - mAnchor.x) * mSize.x, (1 - mAnchor.y) * mSize.y, 0.0f }, {0, 0, -1}, {uvRight, uvTop}}, //右上
		{{ (1 - mAnchor.x) * mSize.x, -mAnchor.y * mSize.y, 0.0f }, {0, 0, -1}, {uvRight, uvBottom}}, //右下
	};

	mVertBuff.Update(vertices, _countof(vertices));
}

GraphicsPipeline Image3D::GetPipeline()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().mDesc;

	desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	desc.BlendState.AlphaToCoverageEnable = false;

	if (static_cast<int32_t>(mBlendMode) > 2) {
		if(static_cast<int32_t>(mBlendMode) > 4) desc.DepthStencilState.DepthEnable = false;
		desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	}

	switch (mBlendMode) {
	case BlendMode::OpaqueAdd:
		desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
		desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		return GraphicsPipeline::GetOrCreate("Image3D_OpaqueAdd", desc);
	case BlendMode::OpaqueSub:
		desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_REV_SUBTRACT;
		desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
		desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		return GraphicsPipeline::GetOrCreate("Image3D_OpaqueSub", desc);
	case BlendMode::TransparentAlpha:
		desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_REV_SUBTRACT;
		desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
		desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		return GraphicsPipeline::GetOrCreate("Image3D_TransparentAlpha", desc);
	case BlendMode::TransparentAdd:
		desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
		desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		return GraphicsPipeline::GetOrCreate("Image3D_TransparentAdd", desc);
	case BlendMode::TransparentSub:
		desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_REV_SUBTRACT;
		desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
		desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		return GraphicsPipeline::GetOrCreate("Image3D_TransparentSub", desc);
	case BlendMode::OverwriteAlpha:
		desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_REV_SUBTRACT;
		desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
		desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		return GraphicsPipeline::GetOrCreate("Image3D_OverwriteAlpha", desc);
	case BlendMode::OverwriteAdd:
		desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
		desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		return GraphicsPipeline::GetOrCreate("Image3D_OverwriteAdd", desc);
	case BlendMode::OverwriteSub:
		desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_REV_SUBTRACT;
		desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
		desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		return GraphicsPipeline::GetOrCreate("Image3D_OverwriteSub", desc);
	default:
		return GraphicsPipeline::GetOrCreate("Image3D_Alpha", desc);
	}
}

Image3D::Image3D()
{
	Init();
}

Image3D::Image3D(TextureHandle texture, Vector2 size, bool forceSize)
{
	mTexture = texture;

	if (forceSize) {
		mSize.x = size.x;
		mSize.y = size.y;
	}
	else {
		mImageScale = size;
		Texture tex = TextureManager::Get(texture);
		mSize.x = tex.mResource->GetDesc().Width / (float)tex.mResource->GetDesc().Height * mImageScale.x;
		mSize.y = mImageScale.y;
	}

	Init();
}

void Image3D::SetTexture(TextureHandle texture)
{
	mTexture = texture;
	Texture tex = TextureManager::Get(texture);
	if (!mForceSize) {
		mSize.x = tex.mResource->GetDesc().Width / (float)tex.mResource->GetDesc().Height * mSize.x;
	}

	mSrcPos = { 0, 0 };
	mTexRect = { static_cast<float>(tex.mResource->GetDesc().Width), static_cast<float>(tex.mResource->GetDesc().Height) };

	mChangeFlag = true;
}

void Image3D::SetSize(Vector2 size, bool forceSize)
{
	mForceSize = forceSize;
	if (forceSize) {
		mSize = size;
	}
	else {
		mImageScale = size;
		Texture tex = TextureManager::Get(mTexture);
		mSize.x = tex.mResource->GetDesc().Width / (float)tex.mResource->GetDesc().Height * mImageScale.x;
		mSize.y = mImageScale.y;
	}

	mChangeFlag = true;
}
	
void Image3D::Init()
{
	//頂点インデックスデータ
	uint32_t indices[] = {
		0, 2, 1,
		1, 2, 3
	};
	mIndexBuff.Init(indices, _countof(indices));

	UpdateVertex();
}

void Image3D::SetAnchor(Vector2 anchor)
{
	mAnchor = anchor;
	mChangeFlag = true;
}

void Image3D::SetTexRect(int32_t srcX, int32_t srcY, int32_t width, int32_t height)
{
	mSrcPos = { static_cast<float>(srcX), static_cast<float>(srcY) };
	mTexRect = { static_cast<float>(width), static_cast<float>(height) };
	mChangeFlag = true;
}

void Image3D::SetTexRect(float srcX, float srcY, float width, float height)
{
	mSrcPos = { srcX, srcY };
	mTexRect = { width, height };
	mChangeFlag = true;
}

void Image3D::TransferBuffer(ViewProjection viewprojection)
{
	if (mChangeFlag) {
		UpdateVertex();
		mChangeFlag = false;
	}

	mMaterial.Transfer(mMaterialBuff.Get());
	mTransform.Transfer(mTransformBuff.Get());
	mViewProjectionBuff->matrix = viewprojection.mMatrix;
}

std::vector<RenderOrder> Image3D::GetRenderOrder()
{
	std::vector<RenderOrder> result;

	RenderOrder order;
	order.pipelineState = GetPipeline().mPtr.Get();
	order.vertBuff = mVertBuff;
	order.indexBuff = mIndexBuff;
	order.indexCount = 6;
	order.rootData = {
		{TextureManager::Get(mTexture).mGpuHandle},
		{RootDataType::SRBUFFER_CBV, mMaterialBuff.mBuff },
		{RootDataType::SRBUFFER_CBV, mTransformBuff.mBuff },
		{RootDataType::SRBUFFER_CBV, mViewProjectionBuff.mBuff },
		{RootDataType::LIGHT},
	};

	result.push_back(order);

	return result;
}

void Image3D::Draw(std::string stageID)
{
	RenderOrder order;
	order.pipelineState = GetPipeline().mPtr.Get();
	order.vertBuff = mVertBuff;
	order.indexBuff = mIndexBuff;
	order.indexCount = 6;
	order.rootData = {
		{TextureManager::Get(mTexture).mGpuHandle},
		{RootDataType::SRBUFFER_CBV, mMaterialBuff.mBuff },
		{RootDataType::SRBUFFER_CBV, mTransformBuff.mBuff },
		{RootDataType::SRBUFFER_CBV, mViewProjectionBuff.mBuff },
		{RootDataType::LIGHT},
	};

	if (stageID.empty()) {
		if (static_cast<int32_t>(mBlendMode) > 2) {
			stageID = "Transparent";
		}
		else {
			stageID = "Opaque";
		}
	}
	Renderer::DrawCall(stageID, order);
}
