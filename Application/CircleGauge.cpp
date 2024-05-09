#include "CircleGauge.h"
#include "Texture.h"
#include "Renderer.h"
#include "Util.h"

void CircleGauge::Init()
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

void CircleGauge::Update()
{
	sprite.mTransform.UpdateMatrix();
	sprite.TransferBuffer();

	mRadianBuffer->radian = Util::AngleToRadian(radian);
}

void CircleGauge::Draw()
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