#include "GameObject.h"
#include "Camera.h"
#include <Renderer.h>
#include "Quaternion.h"

RootSignature GameObject::GameObjectRootSignature()
{
	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	RootParamaters rootParams(6);
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
	//定数バッファ4番(disolve)
	rootParams[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[5].Descriptor.ShaderRegister = 4; //定数バッファ番号
	rootParams[5].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える

	RootSignatureDesc rootDesc = RDirectX::GetDefRootSignature().mDesc;
	rootDesc.RootParamaters = rootParams;
	RootSignature mRootSig = RootSignature::GetOrCreate("gameObjectSignature", rootDesc);
	
	return mRootSig;
}

GraphicsPipeline GameObject::GameObjectPipeLine()
{
	PipelineStateDesc pipedesc = RDirectX::GetDefPipeline().mDesc;
	pipedesc.VS = Shader::GetOrCreate("GameObjectVS", "Shader/GameObject/GameObjectVS.hlsl", "main", "vs_5_0");
	pipedesc.PS = Shader::GetOrCreate("GameObjectPS", "Shader/GameObject/GameObjectPS.hlsl", "main", "ps_5_0");

	pipedesc.pRootSignature = GameObjectRootSignature().mPtr.Get();

	GraphicsPipeline pipe = GraphicsPipeline::GetOrCreate("GameObject", pipedesc);

	return pipe;
}

GameObject::GameObject() : isAlive(true), colliderSize(1.f)
{
	drawerObj = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));
}

Vector3 GameObject::GetPos() const
{
	return obj.mTransform.position;
}

Vector3 GameObject::GetScale() const
{
	return obj.mTransform.scale;
}

bool GameObject::GetIsAlive() const
{
	return isAlive;
}

Vector3 GameObject::GetFrontVec()
{
	//正面ベクトルを取得
	Vector3 frontVec = { 0,0,1 };
	frontVec.Normalize();

	frontVec *= Quaternion::Euler(obj.mTransform.rotation);
	return frontVec;
}
void GameObject::SetDrawCollider(bool isCollider)
{
	isDrawCollider = isCollider;
}

void GameObject::InitCollider()
{
	collider.pos = GetPos();
	//もっともデカいスケールを半径とする
	float maxR = GetScale().x;
	if (maxR < GetScale().y)maxR = GetScale().y;
	if (maxR < GetScale().z)maxR = GetScale().z;
	collider.r = maxR;
}

void GameObject::UpdateCollider()
{
	collider.pos = GetPos();

	//もっともデカいスケールを半径とする
	float maxR = GetScale().x;
	if (maxR < GetScale().y)maxR = GetScale().y;
	if (maxR < GetScale().z)maxR = GetScale().z;
	collider.r = maxR;
	
	//コライダーの大きさが別で設定できるように
	if (colliderSize != 1.0f) {
		collider.r = colliderSize;
	}

	drawerObj.mTransform.position = collider.pos;
	drawerObj.mTransform.scale = { collider.r ,collider.r ,collider.r };

	drawerObj.mTransform.UpdateMatrix();
	drawerObj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

bool GameObject::GetHitBack(GameObject& back, const GameObject& hit)
{
	if (ColPrimitive3D::CheckSphereToSphere(back.collider, hit.collider)) {
		Vector3 repulsionVec = back.GetPos() - hit.GetPos();
		repulsionVec.Normalize();
		repulsionVec.y = 0;

		//一旦これだけ無理やり足す
		back.obj.mTransform.position += repulsionVec;

		//コライダーがもう一度当たらないようにコライダー更新
		back.UpdateCollider();

		return true;
	}
	return false;
}

void GameObject::BrightTransferBuffer(const ViewProjection& view)
{
	obj.TransferBuffer(view);
	AddColor->addColor.x = brightColor.r;
	AddColor->addColor.y = brightColor.g;
	AddColor->addColor.z = brightColor.b;
	AddColor->addColor.w = brightColor.a;
}

void GameObject::BrightDraw(std::string stageID)
{
	GraphicsPipeline pipe = *GetPipeline();

	for (RenderOrder order : GetRenderOrder()) {
		Renderer::DrawCall(stageID, order);
	}
}

void GameObject::BrightDrawTrans(std::string stageID)
{
	GraphicsPipeline pipe = *GetPipelineTrans();

	for (RenderOrder order : GetRenderOrder()) {
		order.pipelineState = pipe.mPtr.Get();
		Renderer::DrawCall(stageID, order);
	}
}

void GameObject::DrawCollider()
{
	//描画しないならスキップ
	if (!isDrawCollider)return;
	
	//パイプラインをワイヤーフレームに
	PipelineStateDesc pipedesc = RDirectX::GetDefPipeline().mDesc;
	pipedesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

	GraphicsPipeline pipe = GraphicsPipeline::GetOrCreate("WireObject", pipedesc);
	for (RenderOrder& order : drawerObj.GetRenderOrder()) {
		order.pipelineState = pipe.mPtr.Get();
		Renderer::DrawCall("Opaque", order);
	}
}

RootSignature* GameObject::GetRootSig()
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

	return &RootSignature::GetOrCreate("PaintedBright", rDesc);
}

GraphicsPipeline* GameObject::GetPipeline()
{
	PipelineStateDesc pDesc = RDirectX::GetDefPipeline().mDesc;
	pDesc.PS = Shader::GetOrCreate("PaintedBrightPS", "Shader/PaintedBright/PaintedBrightPS.hlsl", 
		"main", "ps_5_1");
	pDesc.pRootSignature = GetRootSig()->mPtr.Get();

	return &GraphicsPipeline::GetOrCreate("PaintedBright", pDesc);
}

GraphicsPipeline* GameObject::GetPipelineTrans()
{
	PipelineStateDesc pDesc = RDirectX::GetDefPipeline().mDesc;
	pDesc.PS = Shader::GetOrCreate("PaintedBrightPS", "Shader/PaintedBright/PaintedBrightPS.hlsl",
		"main", "ps_5_1");
	pDesc.pRootSignature = GetRootSig()->mPtr.Get();

	pDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	return &GraphicsPipeline::GetOrCreate("PaintedBrightTrans", pDesc);
}

std::vector<RenderOrder> GameObject::GetRenderOrder()
{
	std::vector<RenderOrder> result;

	for (int32_t i = 0; i < obj.mModel->mData.size(); i++) {
		ModelMesh* data = obj.mModel->mData[i].get();
		std::vector<RootData> rootData = {
			{ TextureManager::Get(data->mMaterial.mTexture).mGpuHandle },
			{ RootDataType::SRBUFFER_CBV, obj.mMaterialBuffMap[data->mMaterial.mName].mBuff },
			{ RootDataType::SRBUFFER_CBV, obj.mTransformBuff.mBuff },
			{ RootDataType::SRBUFFER_CBV, obj.mViewProjectionBuff.mBuff },
			{ RootDataType::LIGHT },
			{ TextureManager::Get(obj.mPaintDissolveMapTex).mGpuHandle },
			{ RootDataType::SRBUFFER_CBV, obj.mPaintDataBuff.mBuff },
			{ RootDataType::SRBUFFER_CBV, AddColor.mBuff }
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