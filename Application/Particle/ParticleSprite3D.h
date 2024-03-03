/*
* @file ParticleSprite3D.h
* @brief Image3Dを描画するパーティクル
*/

#pragma once
#include "ParticleObject.h"
#include <Image3D.h>
#include <Rect.h>

class ParticleSprite3D : public ParticleObject
{
private:
	Image3D image;
	Color color;
	Vector3 velocity;
	Vector3 inVelocity;
	Vector3 stayVelocity;
	Vector3 outVelocity;
	float radian = 0;
	float rotSpeed = 0;
	Vector2 size;
	Vector2 inSize;
	Vector2 staySize;
	Vector2 outSize;
	float inTime;
	float stayTime;
	float outTime;
	float maxTime;
	float timer = 0;
	bool backTrans = false;

public:
	ParticleSprite3D(Vector3 pos, TextureHandle texHandle, RRect texRect, Vector2 texAnchor, Color color, float angle, float rotSpeed, Vector3 inVelocity, Vector3 stayVelocity, Vector3 outVelocity, Vector2 inSize, Vector2 staySize, Vector2 outSize, float inTime, float stayTime, float outTime, bool backDraw);

	void Update() override;
	void Draw() override;

	static void Spawn(Vector3 pos, TextureHandle texHandle, RRect texRect, Vector2 texAnchor, Color color, float angle, float rotSpeed, Vector3 inVelocity, Vector3 stayVelocity, Vector3 outVelocity, Vector2 inSize, Vector2 staySize, Vector2 outSize, float inTime, float stayTime, float outTime, bool backDraw);
};
