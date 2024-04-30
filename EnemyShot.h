#pragma once
#include "GameObject.h"

class EnemyShot : public GameObject
{
public:
	static void LoadResource();

	void Init()override;
	void Update()override;
	void Draw()override;

	Vector3 moveVec;
	Vector3 rotVec;
};

