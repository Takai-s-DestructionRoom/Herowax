#include "DirectionalParticle.h"
#include "Quaternion.h"

void DirectionalParticle::Init()
{
	IEmitter3D::Init();
}

void DirectionalParticle::Update()
{
	for (auto& particle : particles_)
	{
		Vector3 velocityNorm = particle.velo + particle.accel;
		velocityNorm.Normalize();
		particle.rot = Quaternion::LookAt(velocityNorm,{1,0,0}).ToEuler();
	}
	IEmitter3D::Update();

	for (auto& particle : particles_)
	{
		//加速度を速度に加算
		particle.velo += particle.accel;

		//速度による移動
		particle.pos += particle.velo;
	}
}

void DirectionalParticle::Add(uint32_t addNum, float life, Color color, TextureHandle tex,
	float minScale, float maxScale, Vector3 minVelo, Vector3 maxVelo, float accelPower,
	float growingTimer, float endScale, bool isGravity, bool isBillboard, BlendMode blendMode)
{

	IEmitter3D::Add(
		addNum, life, color, tex, minScale, maxScale,
		minVelo, maxVelo, accelPower, {0,0,0}, { 0,0,0 },
		growingTimer, endScale, isGravity, isBillboard,blendMode);
}
