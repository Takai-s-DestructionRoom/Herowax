/*
* @file ParticleExplode.h
* @brief シンプルな爆発っぽいパーティクル
*/

#pragma once
#include "ParticleObject.h"
#include <ModelObj.h>

class ParticleExplode : public ParticleObject
{
public:
	int color;
	float ang;
	float speed;
	float size;
	float maxSize;
	float maxTime;
	float timer = 0;
	float alpha = 1;

	ParticleExplode(Vector3 pos, int color, float ang, float speed, float size, float time);

	void Update() override;
	void Draw() override;

	static void Spawn(Vector3 pos, int color, float ang, float speed, float size, float time);
};
