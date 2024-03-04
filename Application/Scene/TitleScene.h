/*
* @file TitleScene.h
* @brief タイトルシーン
*/

#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "Camera.h"
#include "ModelObj.h"
#include "Sprite.h"

class TitleScene : public IScene
{
public:
	TitleScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	ModelObj skydome;	//天球

	Camera camera = Camera();
	Vector2 mAngle = { 0, 0 };
	LightGroup light;

	Sprite titleLogo;	//タイトル
};
