#include "Sprite.h"
#include "RDirectX.h"
#include "RWindow.h"
#include "Vertex.h"
#include "Renderer.h"

using namespace Microsoft::WRL;

Sprite::Sprite()
{
	Init();
}

Sprite::Sprite(TextureHandle texture, Vector2 anchor)
{
	mTexture = texture;

	//サイズをセットする
	
	mSize.x = (float)TextureManager::Get(mTexture).mResource->GetDesc().Width;
	mSize.y = (float)TextureManager::Get(mTexture).mResource->GetDesc().Height;

	//アンカーポイントをセットする
	mAnchor = anchor;

	Init();
}

void Sprite::UpdateVertex()
{
	Vector2 texSize = {
		static_cast<float>(TextureManager::Get(mTexture).mResource->GetDesc().Width),
		static_cast<float>(TextureManager::Get(mTexture).mResource->GetDesc().Height)
	};

	float uvLeft = mSrcPos.x / texSize.x;
	float uvRight = (mSrcPos.x + mSize.x) / texSize.x;
	float uvTop = mSrcPos.y / texSize.y;
	float uvBottom = (mSrcPos.y + mSize.y) / texSize.y;

	//頂点データ
	VertexPNU vertices[] = {
		{{ -mAnchor.x * mSize.x, (1 - mAnchor.y) * mSize.y, 0.0f}, {}, {uvLeft, uvBottom}}, //左下
		{{ -mAnchor.x * mSize.x, -mAnchor.y * mSize.y, 0.0f }, {}, {uvLeft, uvTop}}, //左上
		{{ (1 - mAnchor.x) * mSize.x, (1 - mAnchor.y) * mSize.y, 0.0f }, {}, {uvRight, uvBottom}}, //右下
		{{ (1 - mAnchor.x) * mSize.x, -mAnchor.y * mSize.y, 0.0f }, {}, {uvRight, uvTop}}, //右上
	};

	mVertBuff.Update(vertices, _countof(vertices));
}

void Sprite::SetTexture(TextureHandle texture)
{
	mTexture = texture;
	mSrcPos = { 0, 0 };
	mSize.x = (float)TextureManager::Get(mTexture).mResource->GetDesc().Width;
	mSize.y = (float)TextureManager::Get(mTexture).mResource->GetDesc().Height;
	mChangeFlag = true;
}

void Sprite::SetAnchor(Vector2 anchor)
{
	mAnchor = anchor;
	mChangeFlag = true;
}

void Sprite::SetTexRect(int32_t srcX, int32_t srcY, int32_t width, int32_t height)
{
	mSrcPos = { static_cast<float>(srcX), static_cast<float>(srcY) };
	mSize = { static_cast<float>(width), static_cast<float>(height) };
	mChangeFlag = true;
}

void Sprite::Init()
{
	//頂点データ
	VertexPNU vertices[] = {
		{{ -mAnchor.x * mSize.x, (1 - mAnchor.y) * mSize.y, 0.0f}, {}, {0.0f, 1.0f}}, //左下
		{{ -mAnchor.x * mSize.x, -mAnchor.y * mSize.y, 0.0f }, {}, {0.0f, 0.0f}}, //左上
		{{ (1 - mAnchor.x) * mSize.x, (1 - mAnchor.y) * mSize.y, 0.0f }, {}, {1.0f, 1.0f}}, //右下
		{{ (1 - mAnchor.x) * mSize.x, -mAnchor.y * mSize.y, 0.0f }, {}, {1.0f, 0.0f}}, //右上
	};

	//頂点インデックスデータ
	uint32_t indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	VertexPNU::CalcNormalVec(vertices, indices, _countof(indices));

	mVertBuff.Init(vertices, _countof(vertices));
	mIndexBuff.Init(indices, _countof(indices));
}

void Sprite::TransferBuffer()
{
	if (mChangeFlag) {
		UpdateVertex();
		mChangeFlag = false;
	}

	mMaterial.Transfer(mMaterialBuff.Get());
	mTransform.Transfer(mTransformBuff.Get());

	Matrix4 matProjection = Matrix4::OrthoGraphicProjection(
		0.0f, (float)RWindow::GetWidth(),
		0.0f, (float)RWindow::GetHeight(),
		0.0f, 1.0f
	);
	
	mViewProjectionBuff.Get()->matrix = matProjection;
}

void Sprite::TransferBuffer(Matrix4 projection)
{
	if (mChangeFlag) {
		UpdateVertex();
		mChangeFlag = false;
	}

	mMaterial.Transfer(mMaterialBuff.Get());
	mTransform.Transfer(mTransformBuff.Get());

	mViewProjectionBuff.Get()->matrix = projection;
}

std::vector<RenderOrder> Sprite::GetRenderOrder()
{
	std::vector<RenderOrder> result;

	RenderOrder order;

	order.anchorPoint = mTransform.position;
	order.vertBuff = mVertBuff;
	order.indexBuff = mIndexBuff;
	order.indexCount = 6;


	order.rootData = {
		{TextureManager::Get(mTexture).mGpuHandle},
		{RootDataType::SRBUFFER_CBV, mMaterialBuff.mBuff},
		{RootDataType::SRBUFFER_CBV, mTransformBuff.mBuff},
		{RootDataType::SRBUFFER_CBV, mViewProjectionBuff.mBuff},
	};

	int32_t blendMode = static_cast<int32_t>(mBlendMode);
	order.mRootSignature = SpriteManager::GetInstance()->GetRootSignature().mPtr.Get();
	order.pipelineState = SpriteManager::GetInstance()->GetGraphicsPipeline(blendMode).mPtr.Get();
	result.push_back(order);

	return result;
}

void Sprite::Draw()
{
	RenderOrder order;

	order.anchorPoint = mTransform.position;
	order.vertBuff = mVertBuff;
	order.indexBuff = mIndexBuff;
	order.indexCount = 6;

	order.rootData = {
		{TextureManager::Get(mTexture).mGpuHandle},
		{RootDataType::SRBUFFER_CBV, mMaterialBuff.mBuff},
		{RootDataType::SRBUFFER_CBV, mTransformBuff.mBuff},
		{RootDataType::SRBUFFER_CBV, mViewProjectionBuff.mBuff},
	};

	int32_t blendMode = static_cast<int32_t>(mBlendMode);
	order.mRootSignature = SpriteManager::GetInstance()->GetRootSignature().mPtr.Get();
	order.pipelineState = SpriteManager::GetInstance()->GetGraphicsPipeline(blendMode).mPtr.Get();

	std::string stageID = "Sprite";
	if (mTransform.position.z < 0) {
		stageID = "BackSprite";
	}
	Renderer::DrawCall(stageID, order);
}

void SpriteManager::Init()
{
	mRootSignature = RDirectX::GetDefRootSignature();

	StaticSamplerDesc samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; //ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX; //ミップマップ最大値
	samplerDesc.MinLOD = 0.0f; //ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //ピクセルシェーダーからだけ見える
	mRootSignature.mDesc.StaticSamplers = StaticSamplerDescs{ samplerDesc };
	mRootSignature.Create();

	mPipelineState = RDirectX::GetDefPipeline();

	mPipelineState.mDesc.VS = Shader("./Shader/Sprite/SpriteVS.hlsl", "main", "vs_5_0");
	mPipelineState.mDesc.PS = Shader("./Shader/Sprite/SpritePS.hlsl", "main", "ps_5_0");

	// ラスタライザの設定
	mPipelineState.mDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	mPipelineState.mDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	mPipelineState.mDesc.RasterizerState.DepthClipEnable = false;
	mPipelineState.mDesc.BlendState.AlphaToCoverageEnable = false;

	mPipelineState.mDesc.DepthStencilState.DepthEnable = false;
	mPipelineState.mDesc.pRootSignature = mRootSignature.mPtr.Get();

	mPipelineStates[0] = mPipelineState;
	mPipelineStates[0].mDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	mPipelineStates[0].mDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	mPipelineStates[0].mDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	mPipelineStates[0].mDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	mPipelineStates[0].mDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	mPipelineStates[0].mDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	mPipelineStates[0].Create();

	mPipelineStates[1] = mPipelineState;
	mPipelineStates[1].mDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	mPipelineStates[1].mDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	mPipelineStates[1].mDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	mPipelineStates[1].mDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	mPipelineStates[1].mDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	mPipelineStates[1].mDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	mPipelineStates[1].Create();

	mPipelineStates[2] = mPipelineState;
	mPipelineStates[2].mDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_REV_SUBTRACT;
	mPipelineStates[2].mDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	mPipelineStates[2].mDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	mPipelineStates[2].mDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	mPipelineStates[2].mDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	mPipelineStates[2].mDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	mPipelineStates[2].Create();
}
