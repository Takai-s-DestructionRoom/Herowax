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

void ModelObj::Draw()
{
	for (std::shared_ptr<ModelMesh> data : mModel->mData) {
		std::vector<RootData> rootData = {
			{ TextureManager::Get(data->mMaterial.mTexture).mGpuHandle },
			{ RootDataType::SRBUFFER_CBV, mMaterialBuffMap[data->mMaterial.mName].mBuff },
			{ RootDataType::SRBUFFER_CBV, mTransformBuff.mBuff },
			{ RootDataType::SRBUFFER_CBV, mViewProjectionBuff.mBuff },
			{ RootDataType::LIGHT }
		};
		

		std::string stage = "Opaque";
		if (data->mMaterial.mColor.a < 1.0f || mTuneMaterial.mColor.a < 1.0f) stage = "Transparent";
		Renderer::DrawCall(stage, &data->mVertBuff.mView, &data->mIndexBuff.mView, static_cast<uint32_t>(data->mIndices.size()), rootData);
	}
	
}
