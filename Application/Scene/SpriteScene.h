/*
* @file SpriteScene.h
* @brief Sprite描画機能のテストをするシーン
* リリースに不要なため削除予定のシーンの一つ
*/

#pragma once
#include "IScene.h"
#include "LightGroup.h"
#include "DebugCamera.h"
#include "ModelObj.h"
#include "Sprite.h"

class SpriteScene : public IScene
{
public:
	SpriteScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:
	Sprite sprite;

	float pos[2] = { RWindow::GetWidth() / 2.0f, RWindow::GetHeight() / 2.0f};
	float rot[3] = { 0, 0, 0 };
	float scale[2] = { 1, 1 };
	Vector2 anchor = { 0.5f, 0.5f };
	int32_t srcX = 0;
	int32_t srcY = 0;
	int32_t width = 1000;
	int32_t height = 1000;
};

