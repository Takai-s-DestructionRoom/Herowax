/*
* @file MainTestScene.h
* @brief 色んなテストをするための落書き帳的シーン
*/

#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"
#include "SRConstBuffer.h"
#include <Cube.h>
#include <PaintableModelObj.h>

class PaintScene : public IScene
{
public:
	PaintScene();
	~PaintScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	DebugCamera camera = DebugCamera({ 0, 0, -5 });
	LightGroup light;

	PaintableModelObj skydome;

	Sprite sprite;

	PaintableModelObj objA;
	PaintableModelObj objB;

	//customize
	float timer = 0;
	Color paintColor = { 1, 1, 1, 1 };
	float dissolveVal = 0;
	Vector3 mAmbient = { 1, 1, 1 };
	Vector3 mDiffuse = { 1, 1, 1 };
	Vector3 mSpecular = { 1, 1, 1 };
};
