#include "HomingParticle2D.h"

HomingParticle2D::HomingParticle2D(Vector2 targetPos)
{
	targetPos_ = targetPos;
}

void HomingParticle2D::Init()
{
	IEmitter2D::Init();
}

void HomingParticle2D::Update()
{
	IEmitter2D::Update();

	for (auto& particle : particles_)
	{
		if (particle.growingTimer.GetRun())
		{
			//加速度を速度に加算
			particle.velo += particle.accel;

			//速度による移動
			particle.pos += particle.velo;

			particle.startPos = particle.pos;
		}
		else if (particle.easeTimer.GetRun())
		{
			//イージングで目的地まで飛ばす
			particle.pos = InQuadVec2(particle.startPos, targetPos_, particle.easeTimer.GetTimeRate());
		}
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
	}

	//毎回頂点情報が変わるので更新する
	vertBuff.Update(vertices);

	//バッファにデータ送信
	TransferBuffer();
	transform.UpdateMatrix();
}

void HomingParticle2D::Add(uint32_t addNum, float life, Color color, TextureHandle tex, float minScale, float maxScale, Vector2 minVelo, Vector2 maxVelo, float accelPower, float minRot, float maxRot, float growingTimer, float endScale, bool isGravity)
{
	IEmitter2D::Add(
		addNum, life, color, tex, minScale, maxScale,
		minVelo, maxVelo, accelPower, minRot, maxRot,
		growingTimer, endScale, isGravity);
}
