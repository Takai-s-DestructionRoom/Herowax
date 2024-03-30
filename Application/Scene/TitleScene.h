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
#include "Bloom.h"
#include "CrossFilter.h"
#include "Distortion.h"

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
	float cameraDist = -20.f;	//注視点からカメラの距離
	Vector2 cameraAngle{};		//カメラアングル
	LightGroup light;

	Sprite titleLogo;	//タイトル

	ModelObj obj;

	//Bloom bloom;
	//CrossFilter crossFilter;
	Distortion distortion;
	bool isPostEffect;
};
