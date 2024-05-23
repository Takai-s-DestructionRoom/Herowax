#include "SimpleParticle2D.h"

void SimpleParticle2D::Init()
{
	IEmitter2D::Init();
}

void SimpleParticle2D::Update()
{
	IEmitter2D::Update();

	for (auto& particle : particles_)
	{
		//加速度を速度に加算
		particle.velo += particle.accel;

		//速度による移動
		particle.pos += particle.velo/* * elapseSpeed_*/;
	}
}

void SimpleParticle2D::Add(uint32_t addNum, float life, Color color,
	TextureHandle tex, float minScale, float maxScale,
	Vector2 minVelo, Vector2 maxVelo, float accelPower,
	float minRot, float maxRot, float growingTimer,
	float endScale, bool isGravity)
{
	IEmitter2D::Add(
		addNum, life, color, tex, minScale, maxScale,
		minVelo, maxVelo, accelPower, minRot, maxRot,
		growingTimer, endScale, isGravity);
}
