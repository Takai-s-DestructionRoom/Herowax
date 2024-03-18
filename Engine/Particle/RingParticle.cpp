#include "RingParticle.h"

void RingParticle::Init()
{
	IEmitter3D::Init();
}

void RingParticle::Update()
{
	IEmitter3D::Update();

	//一度エミッターの座標更新しないとおかしい値が入ってる状態が1Fあるから共通更新処理の後ろに置いてる
	for (auto& particle : particles_)
	{
		//リング用
		particle.radius = Easing::OutQuad(particle.startRadius, particle.endRadius, particle.easeTimer.GetTimeRate());

		//配置してる場所をもとにエミッターからの距離を算出
		Vector2 emit2Particle =
			Vector2(transform.position.x,transform.position.z) - Vector2(particle.pos.x, particle.pos.z);
		emit2Particle.Normalize();
		float pX = transform.position.x + emit2Particle.x * particle.radius;
		float pZ = transform.position.z + emit2Particle.y * particle.radius;

		//高さだけは別で
		particle.pos.y += particle.velo.y;
		if (isGravity_)
		{
			particle.velo.y -= particle.gravity;
		}

		Vector3 pos(pX, particle.pos.y, pZ);

		particle.pos = pos;
	}
}

void RingParticle::AddRing(
	uint32_t addNum, float life, Color color, TextureHandle tex,
	float startRadius, float endRadius, float minScale, float maxScale,
	float minVeloY, float maxVeloY, Vector3 minRot, Vector3 maxRot,
	float growingTimer, bool isGravity, bool isBillboard)
{
	IEmitter3D::AddRing(
		addNum, life, color, tex, startRadius, endRadius, minScale, maxScale,
		minVeloY, maxVeloY, minRot, maxRot, growingTimer, isGravity, isBillboard);
}
