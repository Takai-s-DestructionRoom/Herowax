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
#include "EnemyManager.h"
#include "EnemySpawner.h"
#include "Boss.h"

#include "Level.h"
#include "Wave.h"
#include "GameCamera.h"

#include "IEventScene.h"
#include "ControlUI.h"
#include "BossAppUI.h"

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
	
	Camera minimapCamera = Camera();	//ミニマップ用の仮想カメラ

	GameCamera gameCamera;

	LightGroup light;
	Vector3 ambient;
	
	ControlUI controlUI;
	BossAppUI bossAppTimerUI;
	
	float rot[3] = { 0, 0, 0 };
	float scale[3] = { 1, 1, 1 };
};
