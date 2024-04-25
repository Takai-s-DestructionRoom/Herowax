/*
* @file FailedScene.h
* @brief 失敗シーン
*/

#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "Camera.h"
#include "ModelObj.h"
#include "Easing.h"

class FailedScene : public IScene
{
public:
	FailedScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	ModelObj skydome;		//天球
	ModelObj bossTarget;	//ボスの狙うもの(描画はしない)

	Camera camera = Camera();
	Vector2 mAngle = { 0, 0 };
	LightGroup light;

	Vector2 failedPos;					//FAILED画像の座標
	Easing::EaseTimer floatingTimer;	//ふよふよタイマー

	Vector2 buttonUIPos;				//ボタンUIの座標
	Easing::EaseTimer flashingTimer;	//点滅タイマー

	float cameraDist;		//カメラの距離
	float cameraRot;		//カメラ回転情報
	Easing::EaseTimer cameraRotTimer;	//カメラ回転用タイマー

	bool sceneChange = false;
};