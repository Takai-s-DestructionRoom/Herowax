/*
* @file ParticleSprite.h
* @brief Spriteを画面上に表示するパーティクル
*/

#pragma once
#include "ParticleObject.h"
#include <Sprite.h>
#include <Rect.h>

class ParticleSprite : public ParticleObject
{
public:
	Sprite sprite;
	Color color;
	float ang;
	float speed;
	float startSpeed;
	float endSpeed;
	float size;
	float startSize;
	float endSize;
	float maxTime;
	float timer = 0;

	ParticleSprite(Vector3 pos, TextureHandle texHandle, RRect texRect, Vector2 texAnchor, Color color, float ang, float startSpeed, float endSpeed, float startSize, float endSize, float time);

	void Update() override;
	void Draw() override;

	static void Spawn(Vector3 pos, TextureHandle texHandle, RRect texRect, Vector2 texAnchor, Color color, float ang, float startSpeed, float endSpeed, float startSize, float endSize, float time);
};
