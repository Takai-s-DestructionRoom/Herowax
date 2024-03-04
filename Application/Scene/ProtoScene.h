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

class ProtoScene : public IScene
{
public:
	ProtoScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	ModelObj skydome;
	Player player;

	Camera camera = Camera();
	Vector2 mAngle = { 0, 0 };
	LightGroup light;

	float rot[3] = { 0, 0, 0 };
	float scale[3] = { 1, 1, 1 };
};
