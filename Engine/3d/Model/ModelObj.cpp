#include "ModelObj.h"
#include "LightGroup.h"
#include <Renderer.h>

void ModelObj::TransferBuffer(ViewProjection viewprojection)
{
	int32_t count = 0;
	for (std::shared_ptr<ModelMesh> data : mModel->mData) {
		std::string name = data->mMaterial.mName;
		if (name.empty()) {
			name = "NoNameMaterial_" + count;
			data->mMaterial.mName = name;
			count++;
		}
		Material mat = data->mMaterial;
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
	//viewProjectionBuff.mConstMap->matrix = viewprojection.matrix;
}

std::vector<RenderOrder> ModelObj::GetRenderOrder()
{
	std::vector<RenderOrder> result;

	for (std::shared_ptr<ModelMesh> data : mModel->mData) {
		std::vector<RootData> rootData = {
			{ TextureManager::Get(data->mMaterial.mTexture).mGpuHandle },
			{ RootDataType::SRBUFFER_CBV, mMaterialBuffMap[data->mMaterial.mName].mBuff },
			{ RootDataType::SRBUFFER_CBV, mTransformBuff.mBuff },
			{ RootDataType::SRBUFFER_CBV, mViewProjectionBuff.mBuff },
			{ RootDataType::LIGHT } 
		};

		RenderOrder order;
		order.mRootSignature = RDirectX::GetDefRootSignature().mPtr.Get();
		order.pipelineState = RDirectX::GetDefPipeline().mPtr.Get();
		order.vertView = &data->mVertBuff.mView;
		order.indexView = &data->mIndexBuff.mView;
		order.indexCount = static_cast<uint32_t>(data->mIndices.size());
		order.rootData = rootData;

		result.push_back(order);
	}

	return result;
}

void ModelObj::Draw(std::string stageID)
{
	for (std::shared_ptr<ModelMesh> data : mModel->mData) {
		std::vector<RootData> rootData = {
			{ TextureManager::Get(data->mMaterial.mTexture).mGpuHandle },
			{ RootDataType::SRBUFFER_CBV, mMaterialBuffMap[data->mMaterial.mName].mBuff },
			{ RootDataType::SRBUFFER_CBV, mTransformBuff.mBuff },
			{ RootDataType::SRBUFFER_CBV, mViewProjectionBuff.mBuff },
			{ RootDataType::LIGHT }
		};
		

		if (stageID.empty()) {
			stageID = "Opaque";
			if (data->mMaterial.mColor.a < 1.0f || mTuneMaterial.mColor.a < 1.0f) stageID = "Transparent";
		}
		Renderer::DrawCall(stageID, &data->mVertBuff.mView, &data->mIndexBuff.mView, static_cast<uint32_t>(data->mIndices.size()), rootData);
	}
}
