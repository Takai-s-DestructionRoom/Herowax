#include "Particle3D.h"
#include "Camera.h"
#include <Renderer.h>
#include "Util.h"

//LightGroup* IEmitter3D::sLightGroup = nullptr;

IEmitter3D::IEmitter3D()
{
	Init();
}

void IEmitter3D::Init()
{
	transform.scale = { 0.1f,0.1f,0.1f };
	transform.Transfer(transformBuff.Get());

	addInterval_ = 0;
	maxScale_ = 0;
	minScale_ = 0;
	scaling_ = 0;

	isActive_ = true;	//生成時には有効フラグ立てる

	elapseSpeed_ = 1.f;

	//更新処理でサイズが変わっちゃうから、あらかじめ最大数分作る
	vertices.resize(maxParticle_);
	//それによってバッファの初期化をする
	vertBuff.Init(vertices);

	texture = TextureManager::Get("");
}

void IEmitter3D::Update()
{
	//寿命が尽きたパーティクルを全削除
	for (auto itr = particles_.begin(); itr != particles_.end();) {
		if (itr->aliveTimer.GetEnd()) {
			itr = particles_.erase(itr);
			continue;
		}
		itr++;
	}

	//全パーティクル更新
	for (auto& particle : particles_)
	{
		//生存時間とイージング用タイマーの更新
		particle.aliveTimer.Update(elapseSpeed_);
		particle.easeTimer.Update(elapseSpeed_);
		particle.growingTimer.Update(elapseSpeed_);

		//スケールの線形補間
		if (particle.growingTimer.GetRun())	//拡大タイマーが動いてたら
		{
			particle.scale = Easing::lerp(0.f, particle.startScale, particle.growingTimer.GetTimeRate());
		}
		else
		{
			particle.scale = Easing::lerp(particle.startScale, particle.endScale, particle.easeTimer.GetTimeRate());
		}

		//初期のランダム角度をもとに回す
		particle.rot += particle.plusRot * elapseSpeed_;

		//一回転したら0に戻してあげる
		if (abs(particle.rot.x) >= Util::PI2)
		{
			particle.rot.x = 0.0f;
		}

		if (abs(particle.rot.y) >= Util::PI2)
		{
			particle.rot.y = 0.0f;
		}

		if (abs(particle.rot.z) >= Util::PI2)
		{
			particle.rot.z = 0.0f;
		}

		//重力加算
		particle.pos.y -= particle.gravity * isGravity_ * elapseSpeed_;
	}

	//頂点バッファへデータ転送
	//パーティクルの情報を1つずつ反映
	for (size_t i = 0; i < vertices.size(); i++)
	{
		if (particles_.size() > i)
		{
			VertexParticle vertex;

			//座標
			vertex.pos = particles_[i].pos;
			//回転
			vertex.rot = particles_[i].rot;
			//色
			vertex.color = particles_[i].color;
			//スケール
			vertex.scale = particles_[i].scale;
			//タイマー
			vertex.timer = particles_[i].easeTimer.GetTimeRate();

			assert(vertices.size() >= particles_.size());
			vertices.at(i) = vertex;
		}
		else
		{
			//頂点情報がリセットされないので更新時には毎フレームリセット
			vertices[i].pos = Vector3::ZERO;
			vertices[i].rot = Vector3::ZERO;
			vertices[i].scale = 0.f;
		}
	}

	//毎回頂点情報が変わるので更新する
	vertBuff.Update(vertices);

	if (isBillboard_)
	{
		//ビュー行列の逆行列
		billboardMat = Camera::sNowCamera->mViewProjection.mView;
		billboardMat = -billboardMat;

		//パーティクルでY軸ビルボード使わなそうだから消しとく
		/*if (Y軸ビルボードなら) {
			Vector3 yVec = Vector3(0, 1, 0).GetNormalize();
			Vector3 xVec = Vector3(billboardMat[0][0], billboardMat[0][1], billboardMat[0][2]).GetNormalize();
			Vector3 zVec = xVec.Cross(yVec).GetNormalize();

			billboardMat[1][0] = yVec.x;
			billboardMat[1][1] = yVec.y;
			billboardMat[1][2] = yVec.z;
			billboardMat[2][0] = zVec.x;
			billboardMat[2][1] = zVec.y;
			billboardMat[2][2] = zVec.z;
		}*/

		billboardMat[3][0] = 0;
		billboardMat[3][1] = 0;
		billboardMat[3][2] = 0;
		billboardMat[3][3] = 1;
	}

	//バッファにデータ送信
	TransferBuffer(Camera::sNowCamera->mViewProjection);
	transform.UpdateMatrix();
}

void IEmitter3D::Draw()
{
	if (isBillboard_)
	{
		//パイプライン
		PipelineStateDesc pipedesc = RDirectX::GetDefPipeline().mDesc;
		pipedesc.VS = Shader::GetOrCreate("ParticleBillboard_VS", "Shader/ParticleBillboard/ParticleBillboardVS.hlsl", "main", "vs_5_0");
		pipedesc.PS = Shader::GetOrCreate("ParticleBillboard_PS", "Shader/ParticleBillboard/ParticleBillboardPS.hlsl", "main", "ps_5_0");
		pipedesc.GS = Shader::GetOrCreate("ParticleBillboard_GS", "Shader/ParticleBillboard/ParticleBillboardGS.hlsl", "main", "gs_5_0");

		pipedesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		pipedesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		//pipedesc.RasterizerState.DepthClipEnable = false;
		//pipedesc.BlendState.AlphaToCoverageEnable = false;
		//pipedesc.BlendState.IndependentBlendEnable = false;

		//加算合成
		pipedesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		pipedesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
		pipedesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
		pipedesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		pipedesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		pipedesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

		/*pipedesc.BlendState.RenderTarget[1].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		pipedesc.BlendState.RenderTarget[1].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
		pipedesc.BlendState.RenderTarget[1].DestBlendAlpha = D3D12_BLEND_ONE;
		pipedesc.BlendState.RenderTarget[1].BlendOp = D3D12_BLEND_OP_ADD;
		pipedesc.BlendState.RenderTarget[1].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		pipedesc.BlendState.RenderTarget[1].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;*/

		RootSignature mRootSignature = RDirectX::GetDefRootSignature();

		// ルートパラメータの設定
		DescriptorRange descriptorRange{};
		descriptorRange.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
		descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRange.BaseShaderRegister = 0; //テクスチャレジスタ0番
		descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		RootParamaters rootParams(4);
		//定数バッファ0番(Transform)
		rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
		rootParams[0].Descriptor.ShaderRegister = 0; //定数バッファ番号
		rootParams[0].Descriptor.RegisterSpace = 0; //デフォルト値
		rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
		//定数バッファ1番(Billboard)
		rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
		rootParams[1].Descriptor.ShaderRegister = 1; //定数バッファ番号
		rootParams[1].Descriptor.RegisterSpace = 0; //デフォルト値
		rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
		//定数バッファ2番(ViewProjection)
		rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
		rootParams[2].Descriptor.ShaderRegister = 2; //定数バッファ番号
		rootParams[2].Descriptor.RegisterSpace = 0; //デフォルト値
		rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
		//テクスチャ
		rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[3].DescriptorTable = DescriptorRanges{ descriptorRange };
		rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		mRootSignature.mDesc.RootParamaters = rootParams;
		mRootSignature.Create();
		pipedesc.pRootSignature = mRootSignature.mPtr.Get();

		// 頂点レイアウト
		pipedesc.InputLayout =
		{
			//座標
			{
				"POSITION",										//セマンティック名
				0,												//同名のセマンティックがあるとき使うインデックス
				DXGI_FORMAT_R32G32B32_FLOAT,					//要素数とビット数を表す
				0,												//入力スロットインデックス
				D3D12_APPEND_ALIGNED_ELEMENT,					//データのオフセット地(左のは自動設定)
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		//入力データ種別
				0												//一度に描画するインスタンス数(0でよい)
			},// (1行で書いたほうが見やすい)
			//座標以外に色、テクスチャUVなどを渡す場合はさらに続ける
			//回転情報
			{
				"ROT",0,
				DXGI_FORMAT_R32G32B32_FLOAT,0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
			},
			//色
			{
				"COLOR",0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
			},
			//大きさ
			{
				"TEXCOORD", 0,
				DXGI_FORMAT_R32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			},
			//タイマー
			{
				"TIMER", 0,
				DXGI_FORMAT_R32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			}
		};
		pipedesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

		GraphicsPipeline pipe = GraphicsPipeline::GetOrCreate("ParticleBillboard", pipedesc);
		RenderOrder order;
		order.mRootSignature = mRootSignature.mPtr.Get();
		order.pipelineState = pipe.mPtr.Get();
		order.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		order.vertBuff = vertBuff;
		order.indexCount = static_cast<uint32_t>(vertices.size());
		order.rootData = {
			{RootDataType::SRBUFFER_CBV, transformBuff.mBuff },
			{RootDataType::SRBUFFER_CBV, billboardBuff.mBuff },
			{RootDataType::SRBUFFER_CBV, viewProjectionBuff.mBuff },
			{texture.mGpuHandle}
		};

		Renderer::DrawCall("Opaque", order);
	}
	else
	{
		//パイプライン
		PipelineStateDesc pipedesc = RDirectX::GetDefPipeline().mDesc;
		pipedesc.VS = Shader::GetOrCreate("Particle3D_VS", "Shader/Particle3D/Particle3DVS.hlsl", "main", "vs_5_0");
		pipedesc.PS = Shader::GetOrCreate("Particle3D_PS", "Shader/Particle3D/Particle3DPS.hlsl", "main", "ps_5_0");
		pipedesc.GS = Shader::GetOrCreate("Particle3D_GS", "Shader/Particle3D/Particle3DGS.hlsl", "main", "gs_5_0");

		pipedesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		pipedesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		pipedesc.BlendState.AlphaToCoverageEnable = false;

		RootSignature mRootSignature = RDirectX::GetDefRootSignature();

		// ルートパラメータの設定
		DescriptorRange descriptorRange{};
		descriptorRange.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
		descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRange.BaseShaderRegister = 0; //テクスチャレジスタ0番
		descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		RootParamaters rootParams(4);
		//定数バッファ0番(Transform)
		rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
		rootParams[0].Descriptor.ShaderRegister = 0; //定数バッファ番号
		rootParams[0].Descriptor.RegisterSpace = 0; //デフォルト値
		rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
		//定数バッファ1番(ViewProjection)
		rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
		rootParams[1].Descriptor.ShaderRegister = 1; //定数バッファ番号
		rootParams[1].Descriptor.RegisterSpace = 0; //デフォルト値
		rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
		//定数バッファ2番(Light)
		rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
		rootParams[2].Descriptor.ShaderRegister = 2; //定数バッファ番号
		rootParams[2].Descriptor.RegisterSpace = 0; //デフォルト値
		rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
		//テクスチャ
		rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[3].DescriptorTable = DescriptorRanges{ descriptorRange };
		rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		mRootSignature.mDesc.RootParamaters = rootParams;
		mRootSignature.Create();
		pipedesc.pRootSignature = mRootSignature.mPtr.Get();

		// 頂点レイアウト
		pipedesc.InputLayout =
		{
			//座標
			{
				"POSITION",										//セマンティック名
				0,												//同名のセマンティックがあるとき使うインデックス
				DXGI_FORMAT_R32G32B32_FLOAT,					//要素数とビット数を表す
				0,												//入力スロットインデックス
				D3D12_APPEND_ALIGNED_ELEMENT,					//データのオフセット地(左のは自動設定)
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		//入力データ種別
				0												//一度に描画するインスタンス数(0でよい)
			},// (1行で書いたほうが見やすい)
			//座標以外に色、テクスチャUVなどを渡す場合はさらに続ける
			//回転情報
			{
				"ROT",0,
				DXGI_FORMAT_R32G32B32_FLOAT,0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
			},
			//色
			{
				"COLOR",0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
			},
			//大きさ
			{
				"TEXCOORD", 0,
				DXGI_FORMAT_R32_FLOAT, 0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
			}
		};
		pipedesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

		GraphicsPipeline pipe = GraphicsPipeline::GetOrCreate("Particle3D", pipedesc);
		RenderOrder order;
		order.mRootSignature = mRootSignature.mPtr.Get();
		order.pipelineState = pipe.mPtr.Get();
		order.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		order.vertBuff = vertBuff;
		order.indexCount = static_cast<uint32_t>(vertices.size());
		order.rootData = {
			{RootDataType::SRBUFFER_CBV, transformBuff.mBuff },
			{RootDataType::SRBUFFER_CBV, viewProjectionBuff.mBuff },
			{RootDataType::LIGHT},
			{texture.mGpuHandle}
		};

		Renderer::DrawCall("Opaque", order);
	}
}

void IEmitter3D::TransferBuffer(ViewProjection viewprojection)
{
	transform.Transfer(transformBuff.Get());
	billboardBuff.Get()->matrix = billboardMat;
	viewProjectionBuff->matrix = viewprojection.mMatrix;
	viewProjectionBuff->cameraPos = viewprojection.mEye;
}

void IEmitter3D::Add(uint32_t addNum, float life, Color color, TextureHandle tex,
	float minScale, float maxScale, Vector3 minVelo, Vector3 maxVelo, float accelPower,
	Vector3 minRot, Vector3 maxRot, float growingTimer, float endScale, bool isGravity, bool isBillboard, float rejectRadius)
{
	isGravity_ = isGravity;
	isBillboard_ = isBillboard;
	texture = TextureManager::Get(tex);

	for (uint32_t i = 0; i < addNum; i++)
	{
		//----- エミッターを逐一生成するので生成数はまず超えないから消す -----//
		////指定した最大数超えてたら生成しない
		//if (particles_.size() >= maxParticle_)
		//{
		//	return;
		//}

		//リストに要素を追加
		particles_.emplace_back();
		//追加した要素の参照
		Particle3D& p = particles_.back();

		//エミッターの中からランダムで座標を決定
		float pX = 0, pY = 0, pZ = 0;
		Vector3 randomPos{};

		pX = Util::GetRand(-transform.scale.x, transform.scale.x);
		pY = Util::GetRand(-transform.scale.y, transform.scale.y);
		pZ = Util::GetRand(-transform.scale.z, transform.scale.z);
		randomPos = { pX, pY, pZ };

		//生成しない範囲があるなら
		if (rejectRadius > 0.f)
		{
			//生成しない範囲外になるまで座標決めなおす
			while ((randomPos - transform.position).Length() < rejectRadius)
			{
				pX = Util::GetRand(-transform.scale.x, transform.scale.x);
				pY = Util::GetRand(-transform.scale.y, transform.scale.y);
				pZ = Util::GetRand(-transform.scale.z, transform.scale.z);

				randomPos = { pX, pY, pZ };
			}
		}

		//引数の範囲から大きさランダムで決定
		float sX = Util::GetRand(minScale, maxScale);
		float sY = Util::GetRand(minScale, maxScale);
		float sZ = Util::GetRand(minScale, maxScale);
		Vector3 randomScale(sX, sY, sZ);
		//引数の範囲から飛ばす方向ランダムで決定
		float vX = Util::GetRand(minVelo.x, maxVelo.x);
		float vY = Util::GetRand(minVelo.y, maxVelo.y);
		float vZ = Util::GetRand(minVelo.z, maxVelo.z);
		Vector3 randomVelo(vX, vY, vZ);
		//引数の範囲から回転をランダムで決定
		float rX = Util::GetRand(minRot.x, maxRot.x);
		float rY = Util::GetRand(minRot.y, maxRot.y);
		float rZ = Util::GetRand(minRot.z, maxRot.z);
		Vector3 randomRot(rX, rY, rZ);

		//決まった座標にエミッター自体の座標を足して正しい位置に
		p.pos = randomPos + transform.position;
		//飛んでく方向に合わせて回転
		p.rot = randomRot;
		p.plusRot = p.rot;
		p.velo = randomVelo;
		p.accel = randomVelo * accelPower;
		p.aliveTimer = life;
		p.aliveTimer.Start();
		p.scale = sX;
		p.startScale = p.scale;
		p.endScale = endScale;
		p.color = color;
		//イージング用のタイマーを設定、開始
		p.easeTimer.maxTime_ = life - growingTimer;	//全体の時間がずれないように最初の拡大部分を引く
		p.easeTimer.Start();
		p.growingTimer.maxTime_ = growingTimer;
		p.growingTimer.Start();
	}
}

void IEmitter3D::AddRing(uint32_t addNum, float life, Color color, TextureHandle tex,
	float startRadius, float endRadius, float minScale, float maxScale,
	float minVeloY, float maxVeloY, Vector3 minRot, Vector3 maxRot,
	float growingTimer, float endScale, bool isGravity, bool isBillboard)
{
	isGravity_ = isGravity;
	isBillboard_ = isBillboard;
	texture = TextureManager::Get(tex);

	//キレイに一周させたいので指定した数が最大数超えてたら修正
	uint32_t addNumClamp = Util::Clamp(addNum, 0, maxParticle_);

	for (uint32_t i = 0; i < addNumClamp; i++)
	{
		//----- エミッターを逐一生成するので生成数はまず超えないから消す -----//
		////指定した最大数超えてたら生成しない
		//if (particles_.size() >= maxParticle_)
		//{
		//	return;
		//}

		//リストに要素を追加
		particles_.emplace_back();
		//追加した要素の参照
		Particle3D& p = particles_.back();

		//エミッターを中心と指定された半径をもとに初期座標設定
		float pX = transform.position.x + startRadius * cosf(Util::AngleToRadian((360.f / (float)addNumClamp) * (float)i));
		float pY = transform.position.y;
		float pZ = transform.position.z + startRadius * sinf(Util::AngleToRadian((360.f / (float)addNumClamp) * (float)i));
		Vector3 pos(pX, pY, pZ);
		//引数の範囲から飛ばす方向ランダムで決定
		float vY = Util::GetRand(minVeloY, maxVeloY);
		//引数の範囲から大きさランダムで決定
		float sX = Util::GetRand(minScale, maxScale);
		float sY = Util::GetRand(minScale, maxScale);
		float sZ = Util::GetRand(minScale, maxScale);
		Vector3 randomScale(sX, sY, sZ);
		//引数の範囲から回転をランダムで決定
		float rX = Util::GetRand(minRot.x, maxRot.x);
		float rY = Util::GetRand(minRot.y, maxRot.y);
		float rZ = Util::GetRand(minRot.z, maxRot.z);
		Vector3 randomRot(rX, rY, rZ);

		//決まった座標を代入
		p.pos = pos;
		//飛んでく方向に合わせて回転
		p.rot = randomRot;
		p.plusRot = p.rot;
		p.velo.y = vY;
		p.aliveTimer = life;
		p.aliveTimer.Start();
		p.scale = sX;
		p.startScale = p.scale;
		p.radius = startRadius;
		p.endRadius = endRadius;
		p.endScale = endScale;
		p.color = color;
		//イージング用のタイマーを設定、開始
		p.easeTimer.maxTime_ = life - growingTimer;	//全体の時間がずれないように最初の拡大部分を引く
		p.easeTimer.Start();
		p.growingTimer.maxTime_ = growingTimer;
		p.growingTimer.Start();
	}
}

void IEmitter3D::SetScale(const Vector3& scale)
{
	transform.scale = scale;
	originalScale_ = scale;		//拡縮用に元のサイズを保管
}

void IEmitter3D::SetScalingTimer(float easeTimer)
{
	scalingTimer_.maxTime_ = easeTimer;
}

void IEmitter3D::StartScalingTimer(bool isRun)
{
	if (isRun)
	{
		scalingTimer_.Start();	//設定と同時にタイマーもスタート
	}
	else
	{
		scalingTimer_.ReverseStart();	//設定と同時にタイマーもスタート
	}
}