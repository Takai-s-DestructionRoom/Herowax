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

class ProtoScene : public IScene
{
public:
	ProtoScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	ModelObj skydome;	//天球
	ModelObj ground;	//地面
	Tower tower;
	Player player;
	EnemyManager enemyManager;

	Camera camera = Camera();
	float cameraDist = -20.f;	//注視点からカメラの距離
	Vector2 cameraAngle{};		//カメラアングル
	Vector2 mAngle = { 0, 0 };
	LightGroup light;

	float rot[3] = { 0, 0, 0 };
	float scale[3] = { 1, 1, 1 };
};
