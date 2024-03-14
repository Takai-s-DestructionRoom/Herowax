#include "PaintableModelObj.h"
#include <PaintUtil.h>
#include <Renderer.h>

void PaintableModelObj::SetupPaint()
{
	mSetuped = true;

	for (auto& mesh : mModel->mData) {
		Texture tex = TextureManager::Get(mesh->mMaterial.mTexture);

		UINT width = static_cast<UINT>(tex.mResource.Get()->GetDesc().Width);
		UINT height = tex.mResource.Get()->GetDesc().Height;
		RenderTexture* rt = RenderTarget::CreateRenderTexture(width, height, Color(0, 0, 0, 1));
		tex.Copy(&rt->GetTexture(), RRect(0, static_cast<long>(width), 0, static_cast<long>(height)));
		mRenderTargets.push_back(rt);
	}
}

bool PaintableModelObj::Paint(ColPrimitive3D::Ray ray, TextureHandle brush, Color color, Vector2 size, Matrix4 matrix)
{
	if (!mSetuped) {
		return false;
	}

	int32_t hitMeshIndex = 0;
	int32_t hitIndex = 0;
	float closestDis = FLT_MAX;
	Vector3 closestPos = { 0, 0, 0 };
	Vector2 resultUV;

	for (int32_t mi = 0; mi < mModel->mData.size(); mi++) {
		ModelMesh& mesh = *mModel->mData[mi].get();
		std::vector<VertexPNU> verts = mesh.mVertices;
		for (auto& vert : verts) {
			Float4 tp = vert.pos;
			tp *= mTransform.matrix;
			vert.pos = tp;
			vert.normal *= Matrix4::RotationZXY(mTransform.rotation.x, mTransform.rotation.y, mTransform.rotation.z);
		}

		for (int32_t i = 0; i < mesh.mIndices.size() / 3; i++) {
			VertexPNU vert1 = verts[3*i];
			VertexPNU vert2 = verts[3*i+1];
			VertexPNU vert3 = verts[3*i+2];

			Vector3 p0 = vert1.pos;
			Vector3 p1 = vert2.pos;
			Vector3 p2 = vert3.pos;

			Vector3 v1 = p1 - p0;
			Vector3 v2 = p2 - p0;

			Vector3 normal = v1.Cross(v2);
			normal.Normalize();

			ColPrimitive3D::Triangle tri{ vert1.pos, vert2.pos, vert3.pos, normal };
			
			float outDis;
			Vector3 outInter;
			if (ColPrimitive3D::CheckRayToTriangle(ray, tri, &outDis, &outInter)) {
				if (outDis < closestDis) {
					closestDis = outDis;
					closestPos = outInter;
					hitMeshIndex = mi;
					hitIndex = i;
					resultUV = PaintUtil::CalcPointTexCoord(
						{ vert1.pos, vert2.pos, vert3.pos },
						{ vert1.uv, vert2.uv, vert3.uv },
						outInter, matrix);
				}
				break;
			}
		}
	}

	if (closestDis != FLT_MAX) {
		Sprite* sprite = nullptr;
		for (auto& painter : mPainters) {
			if (!painter.isUsing) {
				painter.isUsing = true;
				sprite = &painter.sprite;
				break;
			}
		}
		if (sprite == nullptr) {
			mPainters.emplace_back();
			sprite = &mPainters.back().sprite;
		}
		sprite->SetTexture(brush);
		sprite->SetAnchor({ 0.5f, 0.5f });

		float brushWidth = static_cast<float>(TextureManager::Get(brush).mResource.Get()->GetDesc().Width);
		float brushHeight = static_cast<float>(TextureManager::Get(brush).mResource.Get()->GetDesc().Height);

		RenderTexture* rt = mRenderTargets[hitMeshIndex];
		float width = static_cast<float>(rt->GetTexture().mResource.Get()->GetDesc().Width);
		float height = static_cast<float>(rt->GetTexture().mResource.Get()->GetDesc().Height);
		
		sprite->mTransform.position = {
			width * resultUV.x,
			height * resultUV.y,
			0.0f
		};
		sprite->mTransform.scale = { size.x / brushWidth / mTransform.scale.x, size.y / brushHeight / mTransform.scale.y, 1.0f };
		sprite->mTransform.rotation.z = Util::AngleToRadian(Util::GetRand(0.0f, 360.0f));
		sprite->mTransform.UpdateMatrix();
		sprite->mMaterial.mColor = color;
		sprite->TransferBuffer(Matrix4::OrthoGraphicProjection(0, width, 0, height, 0.0f, 1.0f));

		for (RenderOrder& order : sprite->GetRenderOrder()) {
			order.renderTargets = { rt->mName };
			order.viewports.push_back(Viewport(width, height, 0, 0, 0.0f, 1.0f));
			order.scissorRects.push_back(RRect(0, static_cast<long>(width), 0, static_cast<long>(height)));
			Renderer::DrawCall("BackSprite", order);
		}
		return true;
	}
    return false;
}

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
			{ RootDataType::LIGHT }
		};
		if (mSetuped) {
			rootData[0].descriptor = mRenderTargets[i]->GetTexture().mGpuHandle;
		}

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

void PaintableModelObj::Draw(std::string stageID)
{
	for (RenderOrder& order : GetRenderOrder()) {
		order.postCommand = [&]{
			for (auto& painter : mPainters) {
				painter.isUsing = false;
			}
		};
		Renderer::DrawCall(stageID, order);
	}
}
