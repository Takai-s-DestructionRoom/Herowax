/*
* @file IEventScene.h
* @brief 各イベントシーンの基底クラス
*/

#pragma once
#include "Camera.h"
#include "Easing.h"

//基本的にはゲームシーンのカメラを奪う感じ
//イベントシーン中に動かしたいものはこのシーン内だったり、
//シーン稼働中フラグが立ってる時にゲームシーン側で動かす的な
class IEventScene
{
public:
	bool isActive = false;	//イベントシーン稼働してるかフラグ
	Camera camera;			//カメラは基本使うと思うのでここで定義
	Easing::EaseTimer eventTimer;	//イベントシーンの発生時間

	virtual ~IEventScene() {}

	virtual void Init(const Vector3 target = {0.f,0.f,0.f}) {}
	virtual void Update() {}
	virtual void Draw() {}
};