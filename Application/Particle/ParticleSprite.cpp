#include "ParticleSprite.h"
#include "TimeManager.h"
#include "Util.h"
#include <Camera.h>
#include <Renderer.h>
#include <SimpleDrawer.h>

using namespace Util;


ParticleSprite::ParticleSprite(Vector3 pos, TextureHandle texHandle, RRect texRect, Vector2 texAnchor, Color color, float ang, float startSpeed, float endSpeed, float startSize, float endSize, float time)
{
	this->sprite.SetTexture(texHandle);
	this->sprite.SetTexRect(texRect.left, texRect.top, texRect.right - texRect.left, texRect.bottom - texRect.top);
	this->sprite.SetAnchor(texAnchor);
	this->pos = pos;
	this->color = color;
	this->ang = ang;
	this->startSpeed = startSpeed;
	this->endSpeed = endSpeed;
	this->startSize = startSize;
	this->endSize = endSize;
	this->maxTime = time;
}

void ParticleSprite::Update()
{
	timer += TimeManager::deltaTime;
	float t = timer / maxTime;
	if (t > 1) t = 1;

	if (t >= 1) {
		isDeleted = true;
		return;
	}

	float fx = 1 - powf(1 - t, 3);
	speed = startSpeed * (1 - fx) + endSpeed * fx;
	size = startSize * (1 - fx) + endSize * fx;
	color.a = 1 - fx;

	float speedX = speed * cosf(AngleToRadian(ang));
	float speedY = speed * sinf(AngleToRadian(ang));

	pos.x += speedX * TimeManager::deltaTime;
	pos.y += speedY * TimeManager::deltaTime;
}

void ParticleSprite::Draw()
{
	Vector2 screenPos = Camera::sNowCamera->mViewProjection.WorldToScreen({ pos.x, pos.y, pos.z });
	
	sprite.mTransform.position = screenPos;
	sprite.mTransform.scale = { size, size, 1 };
	sprite.mTransform.UpdateMatrix();
	
	sprite.TransferBuffer();
	sprite.Draw();
}

void ParticleSprite::Spawn(Vector3 pos, TextureHandle texHandle, RRect texRect, Vector2 texAnchor, Color color, float ang, float startSpeed, float endSpeed, float startSize, float endSize, float time) {
	manageParticleObjList.emplace_back(std::make_unique<ParticleSprite>(pos, texHandle, texRect, texAnchor, color, ang, startSpeed, endSpeed, startSize, endSize, time));
}
