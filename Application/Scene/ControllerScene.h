/*
* @file ControllerScene.h
* @brief キーボードやコントローラー入力が取れるかテストするシーン
* リリースに不要なため削除予定のシーンの一つ
*/

#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "Camera.h"
#include "ModelObj.h"
#include "Sprite.h"

class ControllerScene : public IScene
{
public:
	ControllerScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	ModelObj skydome;
	ModelObj model;

	Camera camera = Camera();
	Vector2 mAngle = { 0, 0 };
	LightGroup light;

	float rot[3] = { 0, 0, 0 };
	float scale[3] = { 1, 1, 1 };
};
