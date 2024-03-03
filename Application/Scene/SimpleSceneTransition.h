/*
* @file SimpleSceneTransition.h
* @brief 矩形が広がるだけのシンプルなシーントランジション
*/

#pragma once
#include "ISceneTransition.h"
#include "Sprite.h"

class SimpleSceneTransition : public ISceneTransition
{
private:
	Sprite shutter;

	bool inProgress = false;
	float timer = 0;
	float shutterScale = 0;
	float shutterAlpha = 0;
	float shutterElapsedTime = 0;

public:
	SimpleSceneTransition();

	virtual void Update() override;
	virtual void Draw() override;

	//トランジションを開ける
	virtual void Open() override;
	//トランジションが開ききっているか
	virtual bool IsOpened() override;
	//トランジションを閉める
	virtual void Close() override;
	//トランジションが閉まりきっているか
	virtual bool IsClosed() override;
	//トランジションが動作中か
	virtual bool InProgress() override;
};

