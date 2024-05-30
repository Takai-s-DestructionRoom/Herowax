/*
* @file WaxSceneTransition.h
* @brief 矩形が広がるだけのシンプルなシーントランジション
*/

#pragma once
#include "ISceneTransition.h"
#include "Sprite.h"
#include "Easing.h"
#include "Brush.h"

class WaxSceneTransition : public ISceneTransition
{
private:
	bool inProgress = false;				//進行中かフラグ
	Easing::EaseTimer openTimer = 0.5f;		//開いてく時のタイマー
	Easing::EaseTimer closeTimer = 0.5f;	//閉じてく時のタイマー

	//いったんロウなしでべたべた塗るやつを作る
	std::vector<Brush> brushs;

	Easing::EaseTimer brushTimer = 0.1f;

	int32_t num = 0;
	int32_t nowNum = 0;

public:
	WaxSceneTransition();

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