#pragma once
#include "IEventScene.h"
#include "Vector2.h"
#include "CircleGauge.h"

class AttackTutorialScene : public IEventScene
{
private:
	Vector2 tutorialUIPos{};

	CircleGauge attackCountGauge;		//動かしたらゲージが伸びて
	CircleGauge collectCountGauge;

	Sprite attackOk;		//okって出したい
	Sprite collectOk;

public:
	AttackTutorialScene();
	~AttackTutorialScene();

	void Init(const Vector3 target) override;
	void Update() override;
	void Draw() override;

	static std::string GetEventCallStr() { return "AttackTutorialScene"; };
};