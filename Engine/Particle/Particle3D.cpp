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
}

void IEmitter3D::Update()
{
	//寿命が尽きたパーティクルを全削除
	for (uint32_t i = 0; i < particles_.size(); i++)
	{
		if (particles_[i].aliveTimer.GetEnd())
		{
			particles_.erase(particles_.begin() + i);
			i--;
		}
	}

	//全パーティクル更新
	for (auto& particle : particles_)
	{
		//生存時間とイージング用タイマーの更新
		particle.aliveTimer.Update(elapseSpeed_);
		particle.easeTimer.Update(elapseSpeed_);
		particle.growingTimer.Update(elapseSpeed_);

		//スケールの線形補間
		float scale;
		if (isGrowing_)	//発生時に拡大する場合
		{
			if (particle.growingTimer.GetRun())	//拡大タイマーが動いてたら
			{
				scale = Easing::lerp(0.f, particle.startScale, particle.growingTimer.GetTimeRate());
			}
			else
			{
				scale = Easing::lerp(particle.startScale, particle.endScale, particle.easeTimer.GetTimeRate());
			}
		}
		else
		{
			particle.scale = Easing::lerp(particle.startScale, particle.endScale, particle.easeTimer.GetTimeRate());
		}

		//加速度を速度に加算
		particle.velo += particle.accel;

		//初期のランダム角度をもとに回す
		if (isRotation_)
		{
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
		}

		//重力加算
		if (isGravity_)
		{
			particle.velo.y += particle.gravity * elapseSpeed_;
		}

		//速度による移動
		particle.pos += particle.velo * elapseSpeed_;
	}

	//頂点バッファへデータ転送
	//パーティクルの情報を1つずつ反映
	for (size_t i = 0; i < particles_.size(); i++)
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

		vertices.at(i) = vertex;
	}

	//毎回頂点数が変わるので初期化しなおす
	vertBuff.Update(vertices);

	//バッファにデータ送信
	TransferBuffer(Camera::sNowCamera->mViewProjection);
	transform.UpdateMatrix();
}

void IEmitter3D::Draw()
{
	//パイプライン
	PipelineStateDesc pipedesc = RDirectX::GetDefPipeline().mDesc;
	pipedesc.VS = Shader::GetOrCreate("Particle3D_VS", "Shader/Particle3DVS.hlsl", "main", "vs_5_0");
	pipedesc.PS = Shader::GetOrCreate("Particle3D_PS", "Shader/Particle3DPS.hlsl", "main", "ps_5_0");
	pipedesc.GS = Shader::GetOrCreate("Particle3D_GS", "Shader/Particle3DGS.hlsl", "main", "gs_5_0");

	pipedesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipedesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipedesc.BlendState.AlphaToCoverageEnable = false;

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
	order.pipelineState = pipe.mPtr.Get();
	order.vertBuff = vertBuff;
	order.rootData = {
		{RootDataType::SRBUFFER_CBV, transformBuff.mBuff },
		{RootDataType::SRBUFFER_CBV, viewProjectionBuff.mBuff },
		{RootDataType::LIGHT},
	};

	Renderer::DrawCall("Opaque", order);
}

void IEmitter3D::TransferBuffer(ViewProjection viewprojection)
{
	transform.Transfer(transformBuff.Get());
	viewProjectionBuff->matrix = viewprojection.mMatrix;
	viewProjectionBuff->cameraPos = viewprojection.mEye;
}

void IEmitter3D::Add(uint32_t addNum, float life, Color color, float minScale, float maxScale,
	Vector3 minVelo, Vector3 maxVelo, float accelPower, Vector3 minRot, Vector3 maxRot, float growingTimer)
{
	for (uint32_t i = 0; i < addNum; i++)
	{
		//指定した最大数超えてたら生成しない
		if (particles_.size() >= maxParticle_)
		{
			return;
		}

		//リストに要素を追加
		particles_.emplace_back();
		//追加した要素の参照
		Particle3D& p = particles_.back();

		//エミッターの中からランダムで座標を決定
		float pX = Util::GetRand(-transform.scale.x, transform.scale.x);
		float pY = Util::GetRand(-transform.scale.y, transform.scale.y);
		float pZ = Util::GetRand(-transform.scale.z, transform.scale.z);
		Vector3 randomPos(pX, pY, pZ);
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
		p.endScale = 0.0f;
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