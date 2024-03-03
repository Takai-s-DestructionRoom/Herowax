/*
* @file SoundScene.h
* @brief RAudioのテストをするためのシーン
* リリースに不要なため削除予定のシーンの一つ
*/

#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "Camera.h"
#include "ModelObj.h"
#include "Sprite.h"

class SoundScene : public IScene
{
public:
	SoundScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	Camera camera = Camera();
	LightGroup light;

	bool loop = false;
	Vector2 playRange;
	Vector2 loopRange;

	float time = 0;
};
