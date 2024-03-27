#include "HomingParticle.h"

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
			particle.pos += particle.velo/* * elapseSpeed_*/;

			particle.startPos = particle.pos;
		}
		else if (particle.easeTimer.GetRun())
		{
			////現在地と目的地の単位ベクトル
			//Vector3 targetToPlayer = targetPos - particle.pos;
			//Vector3 plusVec =
			//	targetToPlayer.GetNormalize();


			//particle.pos += plusVec;

			particle.pos =
				InQuadVec3(particle.startPos, targetPos, particle.easeTimer.GetTimeRate());
		}
	}
}

void HomingParticle::Add(uint32_t addNum, float life, Color color, TextureHandle tex, float minScale, float maxScale, Vector3 minVelo, Vector3 maxVelo, float accelPower, Vector3 minRot, Vector3 maxRot, float growingTimer, float endScale, bool isGravity, bool isBillboard)
{
	IEmitter3D::Add(
		addNum, life, color, tex, minScale, maxScale,
		minVelo, maxVelo, accelPower, minRot, maxRot,
		growingTimer, endScale, isGravity, isBillboard);
}
