/*
* @file IEventScene.h
* @brief 各イベントシーンの基底クラス
*/

#pragma once
#include "Camera.h"

//基本的にはゲームシーンのカメラを奪う感じ
//イベントシーン中に動かしたいものはこのシーン内だったり、
//シーン稼働中フラグが立ってる時にゲームシーン側で動かす的な
class IEventScene
{
public:
	bool isActive;	//イベントシーン稼働してるかフラグ
	Camera camera;	//カメラは基本使うと思うのでここで定義

	IEventScene();

	virtual ~IEventScene() {}

	virtual void Init() {}
	virtual void Update() {}
};