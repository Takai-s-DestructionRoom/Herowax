/*
* @file DirectionalLight.h
* @brief その名の通りさん
*/

#pragma once
#include "Vector3.h"
#include "RConstBuffer.h"

class DirectionalLight
{
public:
	struct LightBuffer
	{
		bool active; // 4byte
		Vector3 lightVec; // 12byte(total 16)
		Vector3 color; // 12byte(total 28)
		float pad; // 4byte(total 32)
	};

	Vector3 mLightVec;
	Color mColor;
	bool mIsActive = false;

	DirectionalLight() : mLightVec(Vector3(0, -1, 0)), mColor(Color(1, 1, 1, 1)) {}
	DirectionalLight(Vector3 lightVec, Color color) : mLightVec(lightVec), mColor(color) {}

	void TransferBuffer(LightBuffer& buffer) {
		buffer.active = mIsActive;
		buffer.lightVec = mLightVec;
		buffer.color = Vector3(mColor.r, mColor.g, mColor.b);
	}
};

