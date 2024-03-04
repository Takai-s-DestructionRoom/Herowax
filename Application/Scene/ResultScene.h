/*
* @file ResultScene.h
* @brief リザルトシーン
*/

#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "Camera.h"
#include "ModelObj.h"
#include "Sprite.h"

class ResultScene : public IScene
{
public:
	ResultScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	ModelObj skydome;	//天球

	Camera camera = Camera();
	Vector2 mAngle = { 0, 0 };
	LightGroup light;

	Sprite ResultLogo;	//タイトル
};
