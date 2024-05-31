#pragma once
#include "Sprite.h"
#include "Easing.h"

class Brush
{
public:
	void Init();
	void Update();
	void Draw();

	void Close(Vector2 pos,Vector2 sizeMin_, Vector2 sizeMax_);
	void Open(Vector2 pos,Vector2 sizeMin_, Vector2 sizeMax_);
	
	bool Brush::GetOpenEnd();
	bool Brush::GetCloseEnd();

	Sprite sprite;
	
	Vector2 sizeMin = {};
	Vector2 sizeMax = {};
private:
	Easing::EaseTimer openTimer = 0.2f;
	Easing::EaseTimer closeTimer = 0.2f;
};

