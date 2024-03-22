#include "SimpleParticle.h"

void SimpleParticle::Init()
{
	IEmitter3D::Init();
}

void SimpleParticle::Update()
{
	IEmitter3D::Update();

	for (auto& particle : particles_)
	{
		//加速度を速度に加算
		particle.velo += particle.accel;

		//速度による移動
		particle.pos += particle.velo/* * elapseSpeed_*/;
	}
}

void SimpleParticle::Add(uint32_t addNum, float life, Color color, TextureHandle tex,
	float minScale, float maxScale,Vector3 minVelo, Vector3 maxVelo, float accelPower,
	Vector3 minRot, Vector3 maxRot,float growingTimer, float endScale, bool isGravity, bool isBillboard)
{
	IEmitter3D::Add(
		addNum, life, color, tex, minScale, maxScale,
		minVelo, maxVelo, accelPower, minRot, maxRot,
		growingTimer, endScale, isGravity, isBillboard);
}
