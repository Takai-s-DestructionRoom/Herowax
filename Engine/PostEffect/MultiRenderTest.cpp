#include "MultiRenderTest.h"
#include <RDirectX.h>
#include <Renderer.h>
#include <RenderTarget.h>

MultiRenderTest::MultiRenderTest()
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

	texA = RenderTarget::CreateRenderTexture(1280, 720, 0x000000, "MultiRenderTestA");
	texB = RenderTarget::CreateRenderTexture(1280, 720, 0x000000, "MultiRenderTestB");
}

void MultiRenderTest::Draw()
{
	*mConstBuff.Get() = mSetting;

	texA->ClearRenderTarget();
	texA->ClearDepthStencil();
	texB->ClearRenderTarget();
	texB->ClearDepthStencil();

	RenderOrder orderA;
	orderA.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	orderA.vertBuff = mVertBuff;
	orderA.indexBuff = mIndexBuff;
	orderA.indexCount = mIndexBuff.GetIndexCount();
	orderA.anchorPoint = Vector3(0, 0, 0);
	orderA.mRootSignature = GetRootSignatureA().mPtr.Get();
	orderA.pipelineState = GetGraphicsPipelineA().mPtr.Get();
	orderA.viewports = {
		{1280, 720, 0, 0, 0.0f, 1.0f},
		{1280, 720, 0, 0, 0.0f, 1.0f}
	};
	orderA.scissorRects = {
		{0, 1280, 0, 720},
		{0, 1280, 0, 720}
	};
	orderA.renderTargets = { texA->mName, texB->mName };
	orderA.rootData = {
		{ RenderTarget::GetRenderTexture("RenderingImage")->GetTexture().mGpuHandle },
	};
	Renderer::DrawCall("PostEffect", orderA);

	RenderOrder orderB;
	orderB.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	orderB.vertBuff = mVertBuff;
	orderB.indexBuff = mIndexBuff;
	orderB.indexCount = mIndexBuff.GetIndexCount();
	orderB.anchorPoint = Vector3(0, 0, 0);
	orderB.mRootSignature = GetRootSignatureB().mPtr.Get();
	orderB.pipelineState = GetGraphicsPipelineB().mPtr.Get();
	orderB.renderTargets = { "RenderingImage" };
	orderB.rootData = {
		{ texA->GetTexture().mGpuHandle },
		{ texB->GetTexture().mGpuHandle }
	};
	Renderer::DrawCall("PostEffect", orderB);
}

RootSignature& MultiRenderTest::GetRootSignatureA()
{
	RootSignatureDesc desc = RDirectX::GetDefRootSignature().mDesc;

	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	RootParamaters rootParams(2);
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //デスクリプタテーブル
	rootParams[0].DescriptorTable = DescriptorRanges{ descriptorRange };
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
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

	return RootSignature::GetOrCreate("MultiRenderTestA", desc);
}

RootSignature& MultiRenderTest::GetRootSignatureB()
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


	RootParamaters rootParams(2);
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //デスクリプタテーブル
	rootParams[0].DescriptorTable = DescriptorRanges{ descriptorRange };
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //デスクリプタテーブル
	rootParams[1].DescriptorTable = DescriptorRanges{ descriptorRangeB };
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
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

	return RootSignature::GetOrCreate("MultiRenderTestB", desc);
}

GraphicsPipeline& MultiRenderTest::GetGraphicsPipelineA()
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

	desc.NumRenderTargets = 2;
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = desc.BlendState.RenderTarget[0];
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	desc.BlendState.RenderTarget[1] = blenddesc;

	desc.VS = Shader::GetOrCreate("MultiRenderTestA_VS", "Shader/MultiRenderTest/MultiRenderTestAVS.hlsl", "main", "vs_5_0");
	desc.PS = Shader::GetOrCreate("MultiRenderTestA_PS", "Shader/MultiRenderTest/MultiRenderTestAPS.hlsl", "main", "ps_5_0");
	desc.pRootSignature = GetRootSignatureA().mPtr.Get();
	return GraphicsPipeline::GetOrCreate("MultiRenderTestA", desc);
}

GraphicsPipeline& MultiRenderTest::GetGraphicsPipelineB()
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

	desc.VS = Shader::GetOrCreate("MultiRenderTestB_VS", "Shader/MultiRenderTest/MultiRenderTestBVS.hlsl", "main", "vs_5_0");
	desc.PS = Shader::GetOrCreate("MultiRenderTestB_PS", "Shader/MultiRenderTest/MultiRenderTestBPS.hlsl", "main", "ps_5_0");
	desc.pRootSignature = GetRootSignatureB().mPtr.Get();
	return GraphicsPipeline::GetOrCreate("MultiRenderTestB", desc);
}
