#include "HomingParticle.h"
#include "WaxManager.h"

HomingParticle::HomingParticle(Vector3 targetPos, bool useSlimeWax)
{
	targetPos_ = targetPos;
	isUseSlimeWax = useSlimeWax;
}

void HomingParticle::Init()
{
	IEmitter3D::Init();
}

void HomingParticle::Update()
{
	IEmitter3D::Update();

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
			particle.pos = InQuadVec3(particle.startPos, targetPos_, particle.easeTimer.GetTimeRate());
		}

		if (particle.easeTimer.GetEnd())
		{
			particle.scale = 0.f;
		}

		if (isUseSlimeWax) {
			WaxManager::GetInstance()->slimeWax.spheres.emplace_back();
			WaxManager::GetInstance()->slimeWax.spheres.back().collider.pos = particle.pos;
			WaxManager::GetInstance()->slimeWax.spheres.back().collider.r = particle.scale;
		}
	}

	//頂点バッファへデータ転送
	//パーティクルの情報を1つずつ反映
	for (size_t i = 0; i < vertices.size(); i++)
	{
		if (particles_.size() > i)
		{
			VertexParticle3D vertex;

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
	}

	//毎回頂点情報が変わるので更新する
	vertBuff.Update(vertices);

	if (isBillboard_)
	{
		//ビュー行列の逆行列
		billboardMat = Camera::sNowCamera->mViewProjection.mView;
		billboardMat = -billboardMat;

		billboardMat[3][0] = 0;
		billboardMat[3][1] = 0;
		billboardMat[3][2] = 0;
		billboardMat[3][3] = 1;
	}

	//バッファにデータ送信
	TransferBuffer(Camera::sNowCamera->mViewProjection);
	transform.UpdateMatrix();
}

void HomingParticle::Add(
	uint32_t addNum, float life, Color color, TextureHandle tex,
	float minScale, float maxScale, Vector3 minVelo, Vector3 maxVelo,
	float accelPower, Vector3 minRot, Vector3 maxRot,
	float growingTimer, float endScale, bool isGravity, bool isBillboard)
{
	IEmitter3D::Add(
		addNum, life, color, tex, minScale, maxScale,
		minVelo, maxVelo, accelPower, minRot, maxRot,
		growingTimer, endScale, isGravity, isBillboard);
}