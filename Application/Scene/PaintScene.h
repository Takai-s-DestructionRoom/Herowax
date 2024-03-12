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

	ModelObj skydome;
	ModelObj sphere;

	Cube hogeObj;
	Sprite sprite;

	//customize
	Color paintColor = { 1, 1, 1, 1 };
	float paintSize = 300;
};