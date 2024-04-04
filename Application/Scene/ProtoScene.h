/*
* @file ProtoScene.h
* @brief プロトタイプ開発するシーン
*/

#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "Camera.h"
#include "ModelObj.h"
#include "Sprite.h"
#include "Player.h"
#include "Tower.h"
#include "EnemyManager.h"
#include "EnemySpawner.h"
#include "EggUI.h"

#include "Level.h"
#include "Wave.h"
#include "BreakableObj.h"

class ProtoScene : public IScene
{
public:
	ProtoScene();

	void Init() override;
	void Update() override;
	void Draw() override;

	void MinimapCameraUpdate();

private:
	ModelObj skydome;	//天球
	Player player;
	Wave wave;
	EggUI eggUI;
	BreakableObj nest;

	Camera minimapCamera = Camera();	//ミニマップ用の仮想カメラ
	float mmCameraDist = -250.f;		//注視点からカメラの距離

	Camera camera = Camera();			//通常カメラ
	float cameraDist = -20.f;			//注視点からカメラの距離
	Vector2 cameraAngle{};				//カメラアングル
	float cameraSpeed = 0.0f;			//カメラの移動速度
	Vector2 mAngle = { 0, 0 };
	LightGroup light;

	float rot[3] = { 0, 0, 0 };
	float scale[3] = { 1, 1, 1 };

	bool changeFPS = false;
};
