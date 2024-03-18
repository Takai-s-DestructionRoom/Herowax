#include "DepthTest.h"
#include <RDirectX.h>
#include <Renderer.h>
#include <RenderTarget.h>

DepthTest::DepthTest()
{
	//頂点データ
	VertexPU vertices[] = {
		{{ -1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, //左下
		{{ -1.0f, +1.0f, 0.0f}, {0.0f, 0.0f}}, //左上
		{{ +1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, //右下
		{{ +1.0f, +1.0f, 0.0f}, {1.0f, 0.0f}}, //右上
	};

	//頂点インデックスデータ
	uint32_t indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	mVertBuff.Init(vertices, _countof(vertices));
	mIndexBuff.Init(indices, _countof(indices));

	tex = RenderTarget::CreateRenderTexture(1280, 720, 0x000000, "");
}

void DepthTest::Draw()
{
	*mConstBuff.Get() = mSetting;

	tex->ClearRenderTarget();
	tex->ClearDepthStencil();

	RenderOrder orderA;
	orderA.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	orderA.vertBuff = mVertBuff;
	orderA.indexBuff = mIndexBuff;
	orderA.indexCount = mIndexBuff.GetIndexCount();
	orderA.anchorPoint = Vector3(0, 0, 0);
	orderA.mRootSignature = GetRootSignatureA().mPtr.Get();
	orderA.pipelineState = GetGraphicsPipelineA().mPtr.Get();
	orderA.renderTargets = { "RenderingImage" };
	orderA.rootData = {
		{ tex->GetTexture().mGpuHandle },
		{ tex->GetDepthTexture().mGpuHandle },
		{ RootDataType::SRBUFFER_CBV, mConstBuff.mBuff }
	};
	orderA.preCommand = [&] {
		RenderTarget::GetRenderTexture("RenderingImage")->GetTexture().Copy(&tex->GetTexture(), RRect(0, 1280, 0, 720));
		RenderTarget::GetRenderTexture("RenderingImage")->GetDepthTexture().Copy(&tex->GetDepthTexture(), RRect(0, 1280, 0, 720));
	};
	Renderer::DrawCall("PostEffect", orderA);

	//RenderOrder orderC;
	//orderC.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	//orderC.vertBuff = mVertBuff;
	//orderC.indexBuff = mIndexBuff;
	//orderC.indexCount = mIndexBuff.GetIndexCount();
	//orderC.anchorPoint = Vector3(0, 0, 0);
	//orderC.mRootSignature = GetRootSignatureA().mPtr.Get();
	//orderC.pipelineState = GetGraphicsPipelineB().mPtr.Get();
	//orderC.renderTargets = { "RenderingImage" };
	//orderC.rootData = {
	//	{ tex->GetTexture().mGpuHandle }
	//};
	//orderC.preCommand = [&] {
	//	RenderTarget::GetRenderTargetTexture("RenderingImage")->ClearRenderTarget();
	//	RenderTarget::GetRenderTargetTexture("RenderingImage")->ClearDepthStencil();
	//};
	//Renderer::DrawCall("PostEffect", orderC);
}

RootSignature& DepthTest::GetRootSignatureA()
{
	RootSignatureDesc desc = RDirectX::GetDefRootSignature().mDesc;

	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	DescriptorRange descriptorRangeB{};
	descriptorRangeB.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
	descriptorRangeB.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeB.BaseShaderRegister = 1; //テクスチャレジスタ1番
	descriptorRangeB.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	RootParamaters rootParams(3);
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //デスクリプタテーブル
	rootParams[0].DescriptorTable = DescriptorRanges{ descriptorRange };
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //デスクリプタテーブル
	rootParams[1].DescriptorTable = DescriptorRanges{ descriptorRangeB };
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //デスクリプタテーブル
	rootParams[2].Constants.RegisterSpace = 0;
	rootParams[2].Constants.ShaderRegister = 0;
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	desc.RootParamaters = rootParams;

	StaticSamplerDesc samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; //ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //リニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX; //ミップマップ最大値
	samplerDesc.MinLOD = 0.0f; //ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //ピクセルシェーダーからだけ見える
	desc.StaticSamplers = StaticSamplerDescs{ samplerDesc };

	return RootSignature::GetOrCreate("DepthTestA", desc);
}

GraphicsPipeline& DepthTest::GetGraphicsPipelineA()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().mDesc;

	desc.InputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	desc.BlendState.AlphaToCoverageEnable = false;
	desc.DepthStencilState.DepthEnable = false;
	desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = desc.BlendState.RenderTarget[0];
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	desc.VS = Shader::GetOrCreate("DepthTestA_VS", "Shader/DepthTest/DepthTestVS.hlsl", "main", "vs_5_0");
	desc.PS = Shader::GetOrCreate("DepthTestA_PS", "Shader/DepthTest/DepthTestPS.hlsl", "main", "ps_5_0");
	desc.pRootSignature = GetRootSignatureA().mPtr.Get();
	return GraphicsPipeline::GetOrCreate("DepthTestA", desc);
}

GraphicsPipeline& DepthTest::GetGraphicsPipelineB()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().mDesc;

	desc.InputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	desc.BlendState.AlphaToCoverageEnable = false;
	desc.DepthStencilState.DepthEnable = false;
	desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	desc.VS = Shader::GetOrCreate("DepthTestB_VS", "Shader/Final/FinalVS.hlsl", "main", "vs_5_0");
	desc.PS = Shader::GetOrCreate("DepthTestB_PS", "Shader/Final/FinalPS.hlsl", "main", "ps_5_0");
	desc.pRootSignature = GetRootSignatureA().mPtr.Get();
	return GraphicsPipeline::GetOrCreate("DepthTestB", desc);
}
