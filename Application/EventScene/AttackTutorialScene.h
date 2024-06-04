#pragma once
#include "IEventScene.h"
#include "Vector2.h"
#include "CircleGauge.h"

class AttackTutorialScene : public IEventScene
{
private:
	Vector2 tutorialUIPos{};

	CircleGauge attackCountGauge;		//動かしたらゲージが伸びて
	CircleGauge attackCountGaugeBack;		//動かしたらゲージが伸びて
	CircleGauge collectCountGauge;
	CircleGauge collectCountGaugeBack;
	CircleGauge skipCountGauge;
	CircleGauge skipCountGaugeBack;

	Sprite attackOk;		//okって出したい
	Sprite collectOk;

	bool isPush;
	Vector2 AbuttonPos{};
	Vector2 AbuttonSize{};

	Vector2 skipPos{};
	Vector2 skipSize{};

	Easing::EaseTimer waitTimer = 0.5f;

public:
	AttackTutorialScene();
	~AttackTutorialScene();

	void Init(const Vector3 target) override;
	void Update() override;
	void Draw() override;

	static std::string GetEventCallStr() { return "AttackTutorialScene"; };
};