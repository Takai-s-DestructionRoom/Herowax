#include "CircleGauge.h"
#include "Texture.h"
#include "Renderer.h"
#include "Util.h"
#include "Camera.h"
#include "RImGui.h"
#include "Parameter.h"

CircleGauge::CircleGauge(bool mode_)
{
	mode = mode_;
}

void CircleGauge::Init(const std::string& title_)
{
	title = title_;

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

	if (title != "CircleGauge_Def") {
		std::map<std::string, std::string> extract = Parameter::Extract(title);
		baseTrans.position = Parameter::GetVector3Data(extract, "位置", baseTrans.position);
		baseTrans.scale = Parameter::GetVector3Data(extract, "大きさ", baseTrans.scale);
		angle = Parameter::GetParam(extract, "回転", angle);
		rate = Parameter::GetParam(extract,"rate", rate);
	}
}

void CircleGauge::Update()
{
	baseMax = 360.f * rate;
	baseMin = 360.f - baseMax;

	nowRadian = baseMin + baseRadian * rate;

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

	mRadianBuffer->radian = Util::AngleToRadian(nowRadian);
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

void CircleGauge::ImGui()
{
	if (RImGui::showImGui) {
		ImGui::Begin(title.c_str());
		ImGui::Text("nowRadian:%f", nowRadian);
		ImGui::Text("baseRadian:%f", baseRadian);
		ImGui::DragFloat("rate", &rate, 0.01f);
		ImGui::DragFloat3("位置", &baseTrans.position.x);
		ImGui::DragFloat3("大きさ", &baseTrans.scale.x, 0.1f);
		ImGui::DragFloat("回転", &angle, 0.01f);
		
		if (ImGui::Button("セーブ")) {
			Parameter::Begin(title);
			Parameter::SaveVector3("位置", baseTrans.position);
			Parameter::SaveVector3("大きさ", baseTrans.scale);
			Parameter::Save("回転", angle);
			Parameter::Save("rate", rate);
			Parameter::End();
		}

		ImGui::End();
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

void CircleGauge::SetSize(Vector2 size)
{
	if (mode) {
		billboard.mImage.SetSize(size);
	}
	else {
		
	}
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
