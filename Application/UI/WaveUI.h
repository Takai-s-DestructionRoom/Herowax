#pragma once
#include "NumDrawer.h"
#include "Sprite.h"
#include "TimerUI.h"

class WaveUI
{
public:
	void LoadResource();

	void Init();
	void Update();
	void Draw();

private:	
	NumDrawer numDrawer;
	Sprite atText;
	Sprite nextwave;
	Sprite tai;

	Sprite enemyAllKillText;
	TimerUI timerUI;

	Sprite waveBase;			//ウェーブ　〇/〇
	NumDrawer nowwaveDrawer;	//今のウェーブ数表示用
	NumDrawer maxwaveDrawer;	//ウェーブ最大数表示用
};