#include "Wax.h"
#include "Camera.h"
#include "Renderer.h"

Wax::Wax():GameObject(),
	waxOriginColor(0.8f, 0.6f, 0.35f, 1.f),
	waxEndColor(0.8f,0.0f,0.f,1.f),
	atkSpeed(1.f),
	atkPower(1),
	igniteTimer(0.25f),
	burningTimer(1.0f),
	extinguishTimer(0.5f),
	solidTimer(1.f),
	disolveValue(1.f)
{
	state = std::make_unique<WaxNormal>();
	obj = ModelObj(Model::Load("./Resources/Model/wax/wax.obj", "wax", true));
	obj.mTuneMaterial.mColor = waxOriginColor;

	TextureManager::Load("./Resources/DissolveMap.png","DissolveMap");
}

bool Wax::GetIsSolidNow()
{
	if (isSolid && isSolided == false)
	{
		return true;
	}
	return false;
}

void Wax::Init(uint32_t power, Vector3 vec,float speed,
	Vector2 range, float size, float atkTime, float solidTime)
{
	//hp = maxHP;
	atkPower = power;
	atkVec = vec * speed;
	atkRange = range;
	atkSize = size;
	atkTimer = atkTime;
	atkTimer.Start();
	solidTimer = solidTime;
	solidTimer.Start();
}

void Wax::Update()
{
	atkTimer.Update();
	solidTimer.Update();

	isSolided = isSolid;	//1F前の状態を保存

	//段々大きくなる
	atkSize = Easing::OutBack(atkTimer.GetTimeRate());
	Vector3 waxScale = { atkRange.x,1.f,atkRange.y };	//蝋の大きさ
	waxScale *= atkSize;
	SetScale(waxScale);									//大きさを反映

	//空中にいる時だけ
	if (isGround == false)
	{
		obj.mTransform.position += atkVec;	//飛んでいく

		//重力加算
		atkVec.y -= gravity;
	}

	//地面に埋ってたら
	if (obj.mTransform.position.y - obj.mTransform.scale.y < 0.f)
	{
		//地面に触れるとこまで移動
		obj.mTransform.position.y = 0.f;
		
		isGround = true;
	}

	//もう少しで固まりそうなら
	if (GetIsSolidLine())
	{
		//もう1色は更新し続け
		obj.mTuneMaterial.mColor = Color::kWhite;

		//トグルでtrueになったら色変えることで点滅
		isFlashing = ((int)(solidTimer.GetTimeRate() * 1000.0f) % 100 > 50);
		if (isFlashing ^ obj.mTuneMaterial.mColor == Color::kWhite)
		{
			obj.mTuneMaterial.mColor = Color::kYellow;
		}
	}
	else
	{
		obj.mTuneMaterial.mColor = waxOriginColor;
	}

	//固まるまでの時間過ぎたら固まる
	if (solidTimer.GetEnd())
	{
		isSolid = true;
		obj.mTuneMaterial.mColor = Color::kLightblue;
	}

	//燃焼周りのステートの更新
	state->Update(this);

	UpdateCollider();

	mDisolveBuff->disolveValue = disolveValue;

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Wax::Draw()
{
	if (isAlive)
	{
		DescriptorRange descriptorRange{};
		descriptorRange.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
		descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRange.BaseShaderRegister = 0; //テクスチャレジスタ0番
		descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		PipelineStateDesc pipedesc = RDirectX::GetDefPipeline().mDesc;
		pipedesc.VS = Shader::GetOrCreate("DisolveVS", "Shader/DisolveVS.hlsl", "main", "vs_5_0");
		pipedesc.PS = Shader::GetOrCreate("DisolvePS", "Shader/DisolvePS.hlsl", "main", "vs_5_0");
		
		RootParamaters rootParams(7);
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
		//テクスチャ(マスクテクスチャ)
		descriptorRange.BaseShaderRegister = 1;	//テクスチャレジスタ1番
		rootParams[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[5].DescriptorTable = DescriptorRanges{ descriptorRange };
		rootParams[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		//定数バッファ4番(disolve)
		rootParams[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
		rootParams[6].Descriptor.ShaderRegister = 4; //定数バッファ番号
		rootParams[6].Descriptor.RegisterSpace = 0; //デフォルト値
		rootParams[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
		
		RootSignatureDesc rootDesc = RDirectX::GetDefRootSignature().mDesc;
		rootDesc.RootParamaters = rootParams;
		RootSignature mRootSig = RootSignature::GetOrCreate("disolveSignature", rootDesc);
	
		pipedesc.pRootSignature = mRootSig.mPtr.Get();

		GraphicsPipeline pipe = GraphicsPipeline::GetOrCreate("Disolve", pipedesc);
		
		for (std::shared_ptr<ModelMesh> data : obj.mModel->mData) {
			std::vector<RootData> rootData = {
				{ RootDataType::SRBUFFER_CBV, obj.mMaterialBuffMap[data->mMaterial.mName].mBuff },
				{ RootDataType::SRBUFFER_CBV, obj.mTransformBuff.mBuff },
				{ RootDataType::SRBUFFER_CBV, obj.mViewProjectionBuff.mBuff },
				{ RootDataType::LIGHT },
				{ TextureManager::Get(data->mMaterial.mTexture).mGpuHandle },
				{ TextureManager::Get("DissolveMap").mGpuHandle},
				{ RootDataType::SRBUFFER_CBV ,mDisolveBuff.mBuff}
			};

			RenderOrder order;
			order.mRootSignature = mRootSig.mPtr.Get();
			order.pipelineState = pipe.mPtr.Get();
			order.rootData = rootData;
			order.vertView = &data->mVertBuff.mView;
			order.indexView = &data->mIndexBuff.mView;
			order.indexCount = static_cast<uint32_t>(data->mIndices.size());

			Renderer::DrawCall("Opaque", order);
		}
	}
}

bool Wax::IsBurning()
{
	return burningTimer.GetRun();
}

bool Wax::IsNormal() 
{
	return !igniteTimer.GetStarted() &&
		!burningTimer.GetStarted() && 
		!extinguishTimer.GetStarted();
}

bool Wax::GetIsSolidLine()
{
	return solidTimer.GetTimeRate() > 0.7f;
}
