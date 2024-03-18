#pragma once
#include "GameObject.h"

class Terrain : public GameObject
{
public:
	Terrain();
	void Init()override;
	void Update()override;
	void Draw()override;
};

