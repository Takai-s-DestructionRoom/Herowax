/*
* @file SpotLight.h
* @brief その名の通りさん
*/

#pragma once
#include "Vector3.h"
#include "Color.h"
#include "Vector2.h"
#include <math.h>
class SpotLight
{
public:
	struct LightBuffer {
		bool active; // 4byte
		Vector3 pos; // 12byte(total 16)
		Vector3 direction; //12byte(total 28)
		float pad; // 4byte(total 32)
		Vector3 color; // 12byte(total 44)
		float pad2; // 4byte(total 48)
		Vector3 atten; //12byte(total 60)
		float pad3; //4byte(total 64)
		Vector2 factorAngleCos; //8byte(total 72)
		float pad4[2]; //8byte(total 80)
	};

	Vector3 mPos;
	Vector3 mDirection;
	Color mColor;
	Vector3 mAtten; //距離減衰係数
	Vector2 mFactorAngle; //減衰角度
	bool mIsActive = false;

	void TransferBuffer(LightBuffer& buffer) {
		buffer.active = mIsActive;
		buffer.pos = mPos;
		buffer.direction = mDirection;
		buffer.color = { mColor.r, mColor.g, mColor.b };
		buffer.atten = mAtten;
		buffer.factorAngleCos.x = cosf(mFactorAngle.x);
		buffer.factorAngleCos.y = cosf(mFactorAngle.y);
	}
};

