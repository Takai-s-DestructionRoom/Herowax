#pragma once
#include "Sprite.h"
#include "Easing.h"

class Brush
{
public:
	void Init();
	void Update();
	void Draw();

	void Start(Vector2 sizeMin_, Vector2 sizeMax_);

	Sprite sprite;
	
	Vector2 sizeMin = {};
	Vector2 sizeMax = {};
private:
	Easing::EaseTimer timer = 0.2f;
};

