#include "RayMarchTestScene.h"
#include "RInput.h"
#include "RImGui.h"
#include "InstantDrawer.h"
#include "TimeManager.h"
#include "Camera.h"
#include "Renderer.h"
#include "Quaternion.h"

void RayMarchTestScene::Init()
{
	InstantDrawer::PreCreate();
	plane = ModelObj(Model::Load("./Resources/Model/plane.obj", "plane", true));
	
	plane.mTransform.scale = { 10,10,10 };
	plane.mTransform.rotation = { Util::PI,0,0 };

	camera.mViewProjection.mEye = { 0, 0, -10 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();

	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
}

void RayMarchTestScene::Update()
{
	InstantDrawer::DrawInit();

	camera.Update();

	Vector3 cameraVec = camera.mViewProjection.mTarget - camera.mViewProjection.mEye;
	cameraVec.Normalize();

	plane.mTransform.position = camera.mViewProjection.mEye + (cameraVec * 5);
	//plane.mTransform.rotation = Quaternion::LookAt(cameraVec).ToEuler();

	plane.TransferBuffer(Camera::sNowCamera->mViewProjection);
	plane.mTransform.UpdateMatrix();
	
	ImGui::SetNextWindowSize({ 300, 200 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("RayMarchTest", NULL, window_flags);
	ImGui::DragFloat3("position", &plane.mTransform.position.x);
	ImGui::DragFloat3("scale", &plane.mTransform.scale.x);
	ImGui::SliderFloat3("rotation", &plane.mTransform.rotation.x,0,Util::PI2);
	ImGui::End();
}

void RayMarchTestScene::Draw()
{
	//plane.Draw();

	GraphicsPipeline pipe = SlimeShaderPipeLine();

	for (std::shared_ptr<ModelMesh> data : plane.mModel->mData) {
		std::vector<RootData> rootData = {
			{ RootDataType::SRBUFFER_CBV, plane.mMaterialBuffMap[data->mMaterial.mName].mBuff },
			{ RootDataType::SRBUFFER_CBV, plane.mTransformBuff.mBuff },
			{ RootDataType::SRBUFFER_CBV, plane.mViewProjectionBuff.mBuff },
			{ RootDataType::LIGHT },
			{ TextureManager::Get(data->mMaterial.mTexture).mGpuHandle }
		};

		RenderOrder order;
		order.mRootSignature = pipe.mDesc.pRootSignature;
		order.pipelineState = pipe.mPtr.Get();
		order.rootData = rootData;
		order.vertView = &data->mVertBuff.mView;
		order.indexView = &data->mIndexBuff.mView;
		order.indexCount = static_cast<uint32_t>(data->mIndices.size());

		Renderer::DrawCall("Opaque", order);
	}

	InstantDrawer::AllUpdate();
	InstantDrawer::AllDraw2D();
}

void RayMarchTestScene::Finalize()
{
}

GraphicsPipeline RayMarchTestScene::SlimeShaderPipeLine()
{
	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	PipelineStateDesc pipedesc = RDirectX::GetDefPipeline().mDesc;
	pipedesc.VS = Shader::GetOrCreate("SlimeShaderVS", "Shader/SlimeShader/SlimeShaderVS.hlsl", "main", "vs_5_0");
	pipedesc.PS = Shader::GetOrCreate("SlimeShaderPS", "Shader/SlimeShader/SlimeShaderPS.hlsl", "main", "ps_5_0");

	RootParamaters rootParams(5);
	//マテリアル
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0; //定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ1番(Transform)
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[1].Descriptor.ShaderRegister = 1; //定数バッファ番号
	rootParams[1].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ2番(ViewProjection)
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[2].Descriptor.ShaderRegister = 2; //定数バッファ番号
	rootParams[2].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ3番(Light)
	rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[3].Descriptor.ShaderRegister = 3; //定数バッファ番号
	rootParams[3].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//テクスチャ(本体テクスチャ)
	descriptorRange.BaseShaderRegister = 0;	//テクスチャレジスタ1番
	rootParams[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParams[4].DescriptorTable = DescriptorRanges{ descriptorRange };
	rootParams[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	RootSignatureDesc rootDesc = RDirectX::GetDefRootSignature().mDesc;
	rootDesc.RootParamaters = rootParams;
	RootSignature mRootSig = RootSignature::GetOrCreate("SlimeShaderSignature", rootDesc);

	pipedesc.pRootSignature = mRootSig.mPtr.Get();

	GraphicsPipeline pipe = GraphicsPipeline::GetOrCreate("SlimeShader", pipedesc);

	return pipe;
}
