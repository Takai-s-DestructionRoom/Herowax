#include "PaintableModelObj.h"
#include <PaintUtil.h>
#include <Renderer.h>

PaintableModelObj::~PaintableModelObj()
{
	if (mSetuped) {
		for (RenderTexture* rt : mRenderTargets) {
			RenderTarget::DeleteRenderTextureAtEndFrame(rt);
		}
	}
}

void PaintableModelObj::SetupPaint()
{
	mSetuped = true;

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

	mRootSignature = &RootSignature::GetOrCreate("PaintableModelObj", rDesc);

	PipelineStateDesc pDesc = RDirectX::GetDefPipeline().mDesc;
	//pDesc.VS = Shader::GetOrCreate("PaintedBasicVS", "Shader/PaintedBasic/PaintedBasicVS.hlsl", "main", "vs_5_0");
	pDesc.PS = Shader::GetOrCreate("PaintedBasicPS", "Shader/PaintedBasic/PaintedBasicPS.hlsl", "main", "ps_5_0");
	pDesc.pRootSignature = mRootSignature->mPtr.Get();

	PipelineStateDesc pDesc2 = SpriteManager::GetInstance()->GetGraphicsPipeline(0).mDesc;
	pDesc2.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	pDesc2.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	pDesc2.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	pDesc2.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	pDesc2.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
	pDesc2.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	GraphicsPipeline::GetOrCreate("PaintEraser", pDesc2);

	mPipeline = &GraphicsPipeline::GetOrCreate("PaintableModelObj", pDesc);

	for (auto& mesh : mModel->mData) {
		Texture tex = TextureManager::Get(mesh->mMaterial.mTexture);

		UINT width = static_cast<UINT>(tex.mResource.Get()->GetDesc().Width);
		UINT height = tex.mResource.Get()->GetDesc().Height;
		RenderTexture* rt = RenderTarget::CreateRenderTexture(width, height, Color(0, 0, 0, 0));
		mRenderTargets.push_back(rt);
	}
}

bool PaintableModelObj::Paint(Vector3 pos, int32_t hitMeshIndex, int32_t hitIndicesIndex, TextureHandle brush, Color color, Vector2 size, Matrix4 matrix)
{
	if (!mSetuped) {
		return false;
	}

	ModelMesh& mesh = *mModel->mData[hitMeshIndex].get();
	std::vector<VertexPNU> verts = mesh.mVertices;
	for (auto& vert : verts) {
		Float4 tp = vert.pos;
		tp *= mTransform.matrix;
		vert.pos = tp;
		vert.normal *= Matrix4::RotationZXY(mTransform.rotation.x, mTransform.rotation.y, mTransform.rotation.z);
	}

	VertexPNU vert1 = verts[3 * hitIndicesIndex];
	VertexPNU vert2 = verts[3 * hitIndicesIndex + 1];
	VertexPNU vert3 = verts[3 * hitIndicesIndex + 2];

	Vector2 resultUV = PaintUtil::CalcPointTexCoord(
		{ vert1.pos, vert2.pos, vert3.pos },
		{ vert1.uv, vert2.uv, vert3.uv },
		pos, matrix);

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
	if (color.a == 0) {
		sprite->mMaterial.mColor.a = 1;
	}
	sprite->TransferBuffer(Matrix4::OrthoGraphicProjection(0, width, 0, height, 0.0f, 1.0f));

	for (RenderOrder& order : sprite->GetRenderOrder()) {
		order.renderTargets = { rt->mName };
		if (color.a == 0) {
			order.pipelineState = GraphicsPipeline::GetOrCreate("PaintEraser", PipelineStateDesc()).mPtr.Get();
		}
		order.viewports.push_back(Viewport(width, height, 0, 0, 0.0f, 1.0f));
		order.scissorRects.push_back(RRect(0, static_cast<long>(width), 0, static_cast<long>(height)));
		Renderer::DrawCall("BackSprite", order);
	}
	return true;
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
		if (color.a == 0) {
			sprite->mMaterial.mColor.a = 1;
		}
		sprite->TransferBuffer(Matrix4::OrthoGraphicProjection(0, width, 0, height, 0.0f, 1.0f));

		for (RenderOrder& order : sprite->GetRenderOrder()) {
			order.renderTargets = { rt->mName };
			if (color.a == 0) {
				order.pipelineState = GraphicsPipeline::GetOrCreate("PaintEraser", PipelineStateDesc()).mPtr.Get();
			}
			order.viewports.push_back(Viewport(width, height, 0, 0, 0.0f, 1.0f));
			order.scissorRects.push_back(RRect(0, static_cast<long>(width), 0, static_cast<long>(height)));
			Renderer::DrawCall("BackSprite", order);
		}
		return true;
	}
    return false;
}

bool PaintableModelObj::GetInfo(ColPrimitive3D::Ray ray,
	PaintableInfo* info_)
{
	PaintableInfo info;

	for (int32_t mi = 0; mi < mModel->mData.size(); mi++) {
		ModelMesh& mesh = *mModel->mData[mi].get();

		std::vector<VertexPNU> verts = mesh.mVertices;
		for (auto& vert : verts) {
			Float4 tp = vert.pos;
			tp *= mTransform.matrix;
			vert.pos = tp;
			vert.normal *= Matrix4::RotationZXY(
				mTransform.rotation.x,
				mTransform.rotation.y,
				mTransform.rotation.z);
		}

		for (int32_t i = 0; i < mesh.mIndices.size() / 3; i++) {
			VertexPNU vert1 = verts[3 * i];
			VertexPNU vert2 = verts[3 * i + 1];
			VertexPNU vert3 = verts[3 * i + 2];

			Vector3 p0 = vert1.pos;
			Vector3 p1 = vert2.pos;
			Vector3 p2 = vert3.pos;

			Vector3 v1 = p1 - p0;
			Vector3 v2 = p2 - p0;

			Vector3 normal = v1.Cross(v2);
			normal.Normalize();

			info.tri.p0 = vert1.pos;
			info.tri.p1 = vert2.pos;
			info.tri.p2 = vert3.pos;
			info.tri.normal = normal;
			
			float outDis;
			Vector3 outInter;
			if (ColPrimitive3D::CheckRayToTriangle(ray, info.tri,
				&outDis, &outInter)) {
				if (outDis < info.closestDis) {
					info.closestDis = outDis;
					info.closestPos = outInter;
					info.hitMeshIndex = mi;
					info.hitIndex = i;
				}
				if (info_ != nullptr) {
					*info_ = info;
				}

				return true;
			}
		}
	}
	return false;
}

std::vector<ColPrimitive3D::Triangle> PaintableModelObj::GetTriangle() const
{
	std::vector<ColPrimitive3D::Triangle> triangles;

	for (int32_t mi = 0; mi < mModel->mData.size(); mi++) {
		ModelMesh& mesh = *mModel->mData[mi].get();

		std::vector<VertexPNU> verts = mesh.mVertices;
		for (auto& vert : verts) {
			Float4 tp = vert.pos;
			tp *= mTransform.matrix;
			vert.pos = tp;
			vert.normal *= Matrix4::RotationZXY(
				mTransform.rotation.x,
				mTransform.rotation.y,
				mTransform.rotation.z);
		}

		for (int32_t i = 0; i < mesh.mIndices.size() / 3; i++) {
			VertexPNU vert1 = verts[3 * i];
			VertexPNU vert2 = verts[3 * i + 1];
			VertexPNU vert3 = verts[3 * i + 2];

			Vector3 p0 = vert1.pos;
			Vector3 p1 = vert2.pos;
			Vector3 p2 = vert3.pos;

			Vector3 v1 = p1 - p0;
			Vector3 v2 = p2 - p0;

			Vector3 normal = v1.Cross(v2);
			normal.Normalize();

			triangles.emplace_back();
			triangles.back().p0 = vert1.pos;
			triangles.back().p1 = vert2.pos;
			triangles.back().p2 = vert3.pos;
			triangles.back().normal = normal;
		}
	}

	return triangles;
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
		mat.mTiling.x *= mTuneMaterial.mTiling.x;
		mat.mTiling.y *= mTuneMaterial.mTiling.y;
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
			rootData.push_back({ mRenderTargets[i]->GetTexture().mGpuHandle });
		}

		RenderOrder order;
		if (mSetuped) {
			order.mRootSignature = mRootSignature->mPtr.Get();
			order.pipelineState = mPipeline->mPtr.Get();
		}
		else {
			order.mRootSignature = RDirectX::GetDefRootSignature().mPtr.Get();
			order.pipelineState = RDirectX::GetDefPipeline().mPtr.Get();
		}
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
