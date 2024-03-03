/*
* @file BGSimple.h
* @brief 背景名[Simple]に相当するクラス
*/

#pragma once
#include "IBackGround.h"
#include "Sprite.h"
#include "Image3D.h"
#include <cstdint>

class BGSimple : public IBackGround
{
public:
	BGSimple();

	void Update() override;
	void Draw() override;

private:
	Sprite mBackGround;

	float mShotTimer = 0;
	int32_t mShotCount = 0;
};

