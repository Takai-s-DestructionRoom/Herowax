#include "ParticleExplode.h"
#include "TimeManager.h"
#include "Util.h"
#include <Camera.h>
#include <Renderer.h>
#include <SimpleDrawer.h>

using namespace Util;

ParticleExplode::ParticleExplode(Vector3 pos, int color, float ang, float speed, float size, float time)
{
	this->pos = pos;
	this->color = color;
	this->ang = ang;
	this->speed = speed;
	this->size = size;
	this->maxSize = size;
	this->maxTime = time;
}

void ParticleExplode::Update()
{
	timer += TimeManager::deltaTime;
	float t = timer / maxTime;
	if (t > 1) t = 1;

	if (t >= 1) {
		isDeleted = true;
		return;
	}

	float fx = 1 - powf(1 - t, 3);
	size = maxSize * (1 - fx);
	alpha = 1 - fx;

	float speedX = speed * cosf(AngleToRadian(ang));
	float speedY = speed * sinf(AngleToRadian(ang));

	pos.x += speedX * TimeManager::deltaTime;
	pos.y += speedY * TimeManager::deltaTime;
}

void ParticleExplode::Draw()
{
	Vector2 screenPos = Camera::sNowCamera->mViewProjection.WorldToScreen({ pos.x, pos.y, pos.z });
	SimpleDrawer::DrawBox(
		static_cast<int32_t>(screenPos.x - size),
		static_cast<int32_t>(screenPos.y - size),
		static_cast<int32_t>(screenPos.x + size),
		static_cast<int32_t>(screenPos.y + size),
		0, color, true
	);
}

void ParticleExplode::Spawn(Vector3 pos, int color, float ang, float speed, float size, float time) {
	manageParticleObjList.emplace_back(std::make_unique<ParticleExplode>(pos, color, ang, speed, size, time));
}
