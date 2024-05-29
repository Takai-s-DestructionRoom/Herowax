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
	TimerUI timerUI;;
};