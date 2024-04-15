#pragma once
#include "IEventScene.h"

class BossAppearanceScene : public IEventScene
{
public:
	~BossAppearanceScene();

	void Init(const Vector3 target) override;
	void Update() override;
	void Draw() override;
};