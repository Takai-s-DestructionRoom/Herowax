#pragma once
#include "GameObject.h"

class Ground : public GameObject
{
public:
	Ground();
	void Init();
	void Update();
	void Draw();
};

