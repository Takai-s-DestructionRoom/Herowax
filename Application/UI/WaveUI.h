#pragma once
#include "NumDrawer.h"
#include "Sprite.h"

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
};