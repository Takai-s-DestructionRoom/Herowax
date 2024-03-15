#include "RingParticle.h"

void RingParticle::Init()
{
	IEmitter3D::Init();
	IEmitter3D::SetIsRotation(true);
}

void RingParticle::Update()
{
	IEmitter3D::Update();
}

void RingParticle::AddRing(uint32_t addNum, float life, Color color, float radius, float minScale, float maxScale, Vector3 minVelo, Vector3 maxVelo, float accelPower, Vector3 minRot, Vector3 maxRot, float growingTimer)
{
	IEmitter3D::AddRing(
		addNum, life, color, radius, minScale, maxScale,
		minVelo, maxVelo, accelPower, minRot, maxRot, growingTimer);
}
