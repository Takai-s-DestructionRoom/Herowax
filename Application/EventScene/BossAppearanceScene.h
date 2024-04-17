#pragma once
#include "IEventScene.h"

class Boss;

class BossAppearanceScene : public IEventScene
{
public:
	Easing::EaseTimer waitTimer;
	Easing::EaseTimer textEaseTimer;

	Vector2 textOriSize;
	Vector2 textSize;

	std::unique_ptr<Boss> boss_;

public:
	~BossAppearanceScene();

	void Init(const Vector3 target) override;
	void Update() override;
	void Draw() override;
};