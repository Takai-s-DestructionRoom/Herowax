#include "CircleGauge.h"
#include "Texture.h"
#include "Renderer.h"
#include "Util.h"
#include "Camera.h"

CircleGauge::CircleGauge(bool mode_)
{
	mode = mode_;
}

void CircleGauge::Init()
{
	if (mode) {
		mRootSignature = RDirectX::GetDefRootSignature();
		mPipelineState = billboard.mImage.GetPipeline();
		
		mPipelineState.mDesc.VS = Shader("./Shader/BillboardRadial360/BillboardRadial360VS.hlsl", "main", "vs_5_0");
		mPipelineState.mDesc.PS = Shader("./Shader/BillboardRadial360/BillboardRadial360PS.hlsl", "main", "ps_5_0");

		//パラメータの追加
		RootParamater radial360RP{};
		radial360RP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		radial360RP.Descriptor.ShaderRegister = 4;
		radial360RP.Descriptor.RegisterSpace = 0;
		radial360RP.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		mRootSignature.mDesc.RootParamaters.push_back(radial360RP);
		
		//深度書き込みをしない(透過されるように)
		mPipelineState.mDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

		mRootSignature.Create();

		mPipelineState.mDesc.pRootSignature = mRootSignature.mPtr.Get();

		//書き換えたので作り直し
		mPipelineState.Create();

		billboard.Init(TextureManager::Load("./Resources/circleGauge.png", "circleGauge"),{10,10});
		billboard.mImage.SetTexture(TextureManager::Load("./Resources/circleGauge.png"));
		billboard.mImage.SetSize({ 10,10 });
		billboard.mImage.mBlendMode = Image3D::BlendMode::TransparentAlpha;
		billboard.mUseBillboardY = false;
	}
	else
	{
		mRootSignature = SpriteManager::GetInstance()->GetRootSignature();
		mPipelineState = SpriteManager::GetInstance()->GetGraphicsPipeline();

		mPipelineState.mDesc.VS = Shader("./Shader/Radial360/Radial360VS.hlsl", "main", "vs_5_0");
		mPipelineState.mDesc.PS = Shader("./Shader/Radial360/Radial360PS.hlsl", "main", "ps_5_0");

		//パラメータの追加
		RootParamater radial360RP{};
		radial360RP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		radial360RP.Descriptor.ShaderRegister = 3;
		radial360RP.Descriptor.RegisterSpace = 0;
		radial360RP.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		mRootSignature.mDesc.RootParamaters.push_back(radial360RP);

		mPipelineState.mDesc.pRootSignature = mRootSignature.mPtr.Get();

		//書き換えたので作り直し
		mPipelineState.Create();

		sprite.Init();
		//デフォテクスチャを配置
		sprite.SetTexture(TextureManager::Load("./Resources/circleGauge.png", "circleGauge"));
	}
}

void CircleGauge::Update()
{
	if (mode) {
		billboard.mTransform = baseTrans;
		billboard.mImage.mTransform.rotation.z = angle;
		billboard.Update(Camera::sNowCamera->mViewProjection);
	}
	else
	{
		baseTrans.rotation.z = angle;
		sprite.mTransform = baseTrans;
		sprite.mTransform.UpdateMatrix();
		sprite.TransferBuffer();
	}

	mRadianBuffer->radian = Util::AngleToRadian(radian);
}

void CircleGauge::Draw()
{
	if (mode) {
		for (RenderOrder& order : billboard.GetRenderOrder())
		{
			order.mRootSignature = GetRootSignature().mPtr.Get();
			order.pipelineState = GetGraphicsPipeline().mPtr.Get();

			order.rootData.push_back({ RootDataType::SRBUFFER_CBV, mRadianBuffer.mBuff });
			
			std::string stageID = "Transparent";
			Renderer::DrawCall(stageID, order);
		}
	}
	else
	{
		for (RenderOrder& order : sprite.GetRenderOrder()) {
			order.mRootSignature = GetRootSignature().mPtr.Get();
			order.pipelineState = GetGraphicsPipeline().mPtr.Get();

			order.rootData.push_back({ RootDataType::SRBUFFER_CBV, mRadianBuffer.mBuff });

			std::string stageID = "Sprite";
			if (sprite.mTransform.position.z < 0) {
				stageID = "BackSprite";
			}
			Renderer::DrawCall(stageID, order);
		}
	}
}

void CircleGauge::SetTexture(TextureHandle texture)
{
	if (mode) {
		billboard.mImage.SetTexture(texture);
	}
	else {
		sprite.SetTexture(texture);
	}
}

void CircleGauge::SetBillboardSize(Vector2 size)
{
	billboard.mImage.SetSize(size);
}

void CircleGauge::SetColor(Color color)
{
	if (mode) {
		billboard.mImage.mMaterial.mColor = color;
	}
	else {
		sprite.mMaterial.mColor = color;
	}
}
