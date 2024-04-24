/*
* @file TitleScene.h
* @brief タイトルシーン
*/

#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "Camera.h"
#include "ModelObj.h"
#include "Easing.h"

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

	Vector2 titleLogoPos;				//タイトルの座標
	Easing::EaseTimer floatingTimer;	//ふよふよタイマー

	Vector2 buttonUIPos;				//ボタンUIの座標
	Easing::EaseTimer flashingTimer;	//点滅タイマー

	float cameraDist;		//カメラの距離
	float cameraRot;		//カメラ回転情報
	Easing::EaseTimer cameraRotTimer;	//カメラ回転用タイマー

	bool sceneChange = false;
};
