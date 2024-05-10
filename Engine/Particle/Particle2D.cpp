#include "Particle2D.h"
#include "Camera.h"
#include <Renderer.h>
#include "Util.h"

IEmitter2D::IEmitter2D()
{
	Init();
}

void IEmitter2D::Init()
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

void IEmitter2D::Update()
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
		if (abs(particle.rot) >= Util::PI2)
		{
			particle.rot = 0.0f;
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
			VertexParticle2D vertex;

			//座標
			vertex.pos = particles_[i].pos;
			//回転
			vertex.rot.z = particles_[i].rot;
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
			vertices[i].pos = Vector2::ZERO;
			vertices[i].rot = Vector3::ZERO;
			vertices[i].scale = 0.f;
		}
	}

	//毎回頂点情報が変わるので更新する
	vertBuff.Update(vertices);

	//バッファにデータ送信
	transform.UpdateMatrix();
	TransferBuffer();
}

void IEmitter2D::Draw()
{
	//パイプライン
	PipelineStateDesc pipedesc = RDirectX::GetDefPipeline().mDesc;
	pipedesc.VS = Shader::GetOrCreate("Particle2D_VS", "Shader/Particle2D/Particle2DVS.hlsl", "main", "vs_5_0");
	pipedesc.PS = Shader::GetOrCreate("Particle2D_PS", "Shader/Particle2D/Particle2DPS.hlsl", "main", "ps_5_0");
	pipedesc.GS = Shader::GetOrCreate("Particle2D_GS", "Shader/Particle2D/Particle2DGS.hlsl", "main", "gs_5_0");

	pipedesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipedesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipedesc.BlendState.AlphaToCoverageEnable = false;

	//加算合成
	pipedesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	pipedesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	pipedesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	pipedesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	pipedesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	pipedesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

	RootSignature mRootSignature = RDirectX::GetDefRootSignature();

	// ルートパラメータの設定
	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	RootParamaters rootParams(3);
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
	//テクスチャ
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParams[2].DescriptorTable = DescriptorRanges{ descriptorRange };
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

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

	GraphicsPipeline pipe = GraphicsPipeline::GetOrCreate("Particle2D", pipedesc);
	RenderOrder order;
	order.mRootSignature = mRootSignature.mPtr.Get();
	order.pipelineState = pipe.mPtr.Get();
	order.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	order.vertBuff = vertBuff;
	order.indexCount = static_cast<uint32_t>(vertices.size());
	order.rootData = {
		{RootDataType::SRBUFFER_CBV, transformBuff.mBuff },
		{RootDataType::SRBUFFER_CBV, viewProjectionBuff.mBuff },
		{texture.mGpuHandle}
	};

	Renderer::DrawCall("Transparent", order);
}

void IEmitter2D::TransferBuffer()
{
	transform.Transfer(transformBuff.Get());

	Matrix4 matProjection = Matrix4::OrthoGraphicProjection(
		0.0f, (float)RWindow::GetWidth(),
		0.0f, (float)RWindow::GetHeight(),
		0.0f, 1.0f
	);

	viewProjectionBuff->matrix = matProjection;
}

void IEmitter2D::Add(uint32_t addNum, float life, Color color, TextureHandle tex,
	float minScale, float maxScale, Vector2 minVelo, Vector2 maxVelo, float accelPower,
	float minRot, float maxRot, float growingTimer, float endScale, bool isGravity, float rejectRadius)
{
	isGravity_ = isGravity;
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
		Particle2D& p = particles_.back();

		//エミッターの中からランダムで座標を決定
		float pX = 0, pY = 0;
		Vector2 randomPos{};

		pX = Util::GetRand(-transform.scale.x, transform.scale.x);
		pY = Util::GetRand(-transform.scale.y, transform.scale.y);
		randomPos = { pX, pY };

		//生成しない範囲があるなら
		if (rejectRadius > 0.f)
		{
			//生成しない範囲外になるまで座標決めなおす
			while ((randomPos - transform.position).Length() < rejectRadius)
			{
				pX = Util::GetRand(-transform.scale.x, transform.scale.x);
				pY = Util::GetRand(-transform.scale.y, transform.scale.y);

				randomPos = { pX, pY };
			}
		}

		//引数の範囲から大きさランダムで決定
		float sX = Util::GetRand(minScale, maxScale);
		float sY = Util::GetRand(minScale, maxScale);
		Vector2 randomScale(sX, sY);
		//引数の範囲から飛ばす方向ランダムで決定
		float vX = Util::GetRand(minVelo.x, maxVelo.x);
		float vY = Util::GetRand(minVelo.y, maxVelo.y);
		Vector2 randomVelo(vX, vY);
		//引数の範囲から回転をランダムで決定
		float randomRot = Util::GetRand(minRot, maxRot);

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

void IEmitter2D::SetScale(const Vector2& scale)
{
	transform.scale = scale;
	originalScale_ = scale;		//拡縮用に元のサイズを保管
}

void IEmitter2D::SetScalingTimer(float easeTimer)
{
	scalingTimer_.maxTime_ = easeTimer;
}

void IEmitter2D::StartScalingTimer(bool isRun)
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