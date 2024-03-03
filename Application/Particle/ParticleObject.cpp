#include "ParticleObject.h"
#include <SimpleDrawer.h>

std::list<std::unique_ptr<ParticleObject>> ParticleObject::manageParticleObjList;

void ParticleObject::ManageParticleObject(bool all)
{
	for (std::list<std::unique_ptr<ParticleObject>>::iterator itr = manageParticleObjList.begin(); itr != manageParticleObjList.end();) {
		ParticleObject* obj = (*itr).get();
		if (obj->isDeleted) {
			itr = manageParticleObjList.erase(itr);
			continue;
		}

		if (obj->isForce || all) {
			obj->Update();
			obj->Draw();
		}
		itr++;
	}
}

void ParticleObject::Clear()
{
	manageParticleObjList.clear();
}

RootSignature& ParticleObject::GetRootSignature()
{
	RootSignatureDesc desc = RDirectX::GetDefRootSignature().mDesc;
	
	RootParamaters rootParams(4);
	//テクスチャレジスタ0番
	rootParams[0] = desc.RootParamaters[0];
	rootParams[1] = desc.RootParamaters[1];
	rootParams[2] = desc.RootParamaters[2];
	rootParams[3] = desc.RootParamaters[3];
	desc.RootParamaters = rootParams;
	return RootSignature::GetOrCreate("ParticleObject", desc);
}

GraphicsPipeline& ParticleObject::GetGraphicsPipeline()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().mDesc;

	desc.BlendState.AlphaToCoverageEnable = false;
	desc.DepthStencilState.DepthEnable = true;
	desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = desc.BlendState.RenderTarget[0];
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_ONE;
	blenddesc.DestBlend = D3D12_BLEND_ONE;
	desc.VS = Shader::GetOrCreate("NoneShadingVS", "Shader/NoneShadingVS.hlsl", "main", "vs_5_0");
	desc.PS = Shader::GetOrCreate("NoneShadingPS", "Shader/NoneShadingPS.hlsl", "main", "ps_5_0");
	desc.pRootSignature = GetRootSignature().mPtr.Get();
	return GraphicsPipeline::GetOrCreate("ParticleObject", desc);
}

GraphicsPipeline& ParticleObject::GetGraphicsPipelineAlpha()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().mDesc;

	desc.BlendState.AlphaToCoverageEnable = false;
	desc.DepthStencilState.DepthEnable = true;
	desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = desc.BlendState.RenderTarget[0];
	blenddesc.BlendEnable = true;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	desc.VS = Shader::GetOrCreate("NoneShadingVS", "Shader/NoneShadingVS.hlsl", "main", "vs_5_0");
	desc.PS = Shader::GetOrCreate("NoneShadingPS", "Shader/NoneShadingPS.hlsl", "main", "ps_5_0");
	desc.pRootSignature = GetRootSignature().mPtr.Get();
	return GraphicsPipeline::GetOrCreate("ParticleObjectAlpha", desc);
}
