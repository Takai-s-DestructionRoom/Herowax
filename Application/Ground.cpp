#include "Ground.h"
#include "Renderer.h"
#include "RImGui.h"
#include "Parameter.h"

void Ground::Init()
{
	obj = ModelObj(Model::Load("./Resources/Model/Ground/Ground.obj", "Ground"));
	
	//座標を設定
	obj.mTransform.position = {0,0,0};
	obj.mTransform.scale = { 10,10,10 };
	obj.mTransform.rotation = { 0,0,0 };

	mRootSignature = RDirectX::GetDefRootSignature();
	mPipelineState = RDirectX::GetDefPipeline();

	mPipelineState.mDesc.VS = Shader("./Shader/TilingBasic/TilingBasicVS.hlsl", "main", "vs_5_0");
	mPipelineState.mDesc.PS = Shader("./Shader/TilingBasic/TilingBasicPS.hlsl", "main", "ps_5_0");

	//パラメータの追加
	RootParamater tilingBasicRP{};
	tilingBasicRP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	tilingBasicRP.Descriptor.ShaderRegister = 4; //定数バッファ番号
	tilingBasicRP.Descriptor.RegisterSpace = 0; //デフォルト値
	tilingBasicRP.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	mRootSignature.mDesc.RootParamaters.push_back(tilingBasicRP);

	mRootSignature.Create();

	mPipelineState.mDesc.pRootSignature = mRootSignature.mPtr.Get();

	////書き換えたので作り直し
	mPipelineState.Create();

	std::map<std::string, std::string> extract = Parameter::Extract("ground");
	tiling.x = Parameter::GetParam(extract,"tilingX", tiling.x);
	tiling.y = Parameter::GetParam(extract,"tilingY", tiling.y);
}

void Ground::Update()
{
	if (RImGui::showImGui)
	{
		ImGui::SetNextWindowSize({ 300, 200 }, ImGuiCond_FirstUseEver);

		ImGui::Begin("tiling");
		ImGui::DragFloat2("tiling", &tiling.x);

		if (ImGui::Button("セーブ")) {
			Parameter::Begin("ground");
			Parameter::Save("tilingX", tiling.x);
			Parameter::Save("tilingY", tiling.y);
			Parameter::End();
		}

		ImGui::End();
	}

	tilingBuffer->tiling = tiling;

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Ground::Draw()
{
	for (RenderOrder& order : obj.GetRenderOrder()) {
		order.mRootSignature = GetRootSignature().mPtr.Get();
		order.pipelineState = GetGraphicsPipeline().mPtr.Get();

		order.rootData.push_back({ RootDataType::SRBUFFER_CBV, tilingBuffer.mBuff });

		Renderer::DrawCall("Opaque", order);
	}
}

void Ground::SetStatus(const Vector3& pos, const Vector3& size, const Vector3& rot)
{
	//座標を設定
	obj.mTransform.position = pos;
	obj.mTransform.scale = size;
	obj.mTransform.rotation = rot;
}

Transform Ground::GetTransform()
{
	return obj.mTransform;
}
