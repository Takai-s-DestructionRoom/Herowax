#pragma once
#include "Vector3.h"
#include "Vector2.h"
#include "Easing.h"

class WaxUI
{
public:
	//ロウが増えたことを示すUI
	void Init();
	void Update(Vector3 target);
	void Draw();

	void Start();

private:
	Vector3 upPaintPosStart;
	Vector3 upPaintPosEnd;
	Vector3 upPaintPos;

	Vector2 upPaintSize = { 0,0 };
	Vector2 upPaintSizeStart = { 2.5f,2.5f };

	float angle = 0;

	Easing::EaseTimer posTimer = 0.75f;
	Easing::EaseTimer sizeTimer = 0.5f;
};

