#include "PaintableModelObj.h"
#include <PaintUtil.h>
#include <Renderer.h>

void PaintableModelObj::TransferBuffer(ViewProjection viewprojection)
{
	int32_t count = 0;
	for (int32_t i = 0; i < mModel->mData.size(); i++) {
		ModelMesh& data = *mModel->mData[i];
		std::string name = data.mMaterial.mName;
		if (name.empty()) {
			name = "NoNameMaterial_" + count;
			data.mMaterial.mName = name;
			count++;
		}
		Material mat = data.mMaterial;
		mat.mColor.r *= mTuneMaterial.mColor.r;
		mat.mColor.g *= mTuneMaterial.mColor.g;
		mat.mColor.b *= mTuneMaterial.mColor.b;
		mat.mColor.a *= mTuneMaterial.mColor.a;
		mat.mAmbient *= mTuneMaterial.mAmbient;
		mat.mDiffuse *= mTuneMaterial.mDiffuse;
		mat.mSpecular *= mTuneMaterial.mSpecular;
		mat.Transfer(mMaterialBuffMap[name].Get());
	}

	mTransform.Transfer(mTransformBuff.Get());
	viewprojection.Transfer(mViewProjectionBuff.Get());
}

std::vector<RenderOrder> PaintableModelObj::GetRenderOrder()
{
	std::vector<RenderOrder> result;

	for (int32_t i = 0; i < mModel->mData.size(); i++) {
		ModelMesh* data = mModel->mData[i].get();
		std::vector<RootData> rootData = {
			{ TextureManager::Get(data->mMaterial.mTexture).mGpuHandle },
			{ RootDataType::SRBUFFER_CBV, mMaterialBuffMap[data->mMaterial.mName].mBuff },
			{ RootDataType::SRBUFFER_CBV, mTransformBuff.mBuff },
			{ RootDataType::SRBUFFER_CBV, mViewProjectionBuff.mBuff },
			{ RootDataType::LIGHT },
			{ TextureManager::Get(mPaintDissolveMapTex).mGpuHandle },
			{ RootDataType::SRBUFFER_CBV, mPaintDataBuff.mBuff }
		};

		RenderOrder order;
		order.mRootSignature = GetRootSig()->mPtr.Get();
		order.pipelineState = GetPipeline()->mPtr.Get();
		order.vertView = &data->mVertBuff.mView;
		order.indexView = &data->mIndexBuff.mView;
		order.indexCount = static_cast<uint32_t>(data->mIndices.size());
		order.rootData = rootData;

		result.push_back(order);
	}

	return result;
}

void PaintableModelObj::Draw(std::string stageID)
{
	for (RenderOrder& order : GetRenderOrder()) {
		Renderer::DrawCall(stageID, order);
	}
}

RootSignature* PaintableModelObj::GetRootSig()
{
	RootSignatureDesc rDesc = RDirectX::GetDefRootSignature().mDesc;

	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1;
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 1; //テクスチャレジスタ1番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	RootParamater paintTexRP{};
	paintTexRP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	paintTexRP.DescriptorTable = { descriptorRange };
	paintTexRP.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rDesc.RootParamaters.push_back(paintTexRP);

	RootParamater paintDataRP{};
	paintDataRP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	paintDataRP.Descriptor.ShaderRegister = 10;
	paintDataRP.Descriptor.RegisterSpace = 0;
	paintDataRP.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rDesc.RootParamaters.push_back(paintDataRP);

	RootParamater paintDataRP2{};
	paintDataRP2.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	paintDataRP2.Descriptor.ShaderRegister = 11;
	paintDataRP2.Descriptor.RegisterSpace = 0;
	paintDataRP2.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rDesc.RootParamaters.push_back(paintDataRP2);

	return &RootSignature::GetOrCreate("PaintableModelObj", rDesc);
}

GraphicsPipeline* PaintableModelObj::GetPipeline()
{
	PipelineStateDesc pDesc = RDirectX::GetDefPipeline().mDesc;
	pDesc.PS = Shader::GetOrCreate("PaintedBasicPS", "Shader/PaintedBasic/PaintedBasicPS.hlsl", "main", "ps_5_1");
	pDesc.pRootSignature = GetRootSig()->mPtr.Get();

	return &GraphicsPipeline::GetOrCreate("PaintableModelObj", pDesc);
}
