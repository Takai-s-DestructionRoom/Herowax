#pragma once
#include "IEventScene.h"
#include "Vector2.h"
#include "CircleGauge.h"

class MoveTutorialScene : public IEventScene
{
private:
	Vector2 tutorialUIPos{};

	CircleGauge moveCountGauge;		//動かしたらゲージが伸びて
	CircleGauge moveCountGaugeBack;
	CircleGauge cameraCountGauge;	
	CircleGauge cameraCountGaugeBack;
	CircleGauge skipCountGauge;
	CircleGauge skipCountGaugeBack;

	Sprite moveOk;		//okって出したい
	Sprite cameraOk;	

	bool isPush;
	Vector2 AbuttonPos{};
	Vector2 AbuttonSize{};

	Vector2 skipPos{};
	Vector2 skipSize{};

	Easing::EaseTimer waitTimer = 0.5f;

public:
	MoveTutorialScene();
	~MoveTutorialScene();

	void Init(const Vector3 target) override;
	void Update() override;
	void Draw() override;

	static std::string GetEventCallStr() { return "MoveTutorialScene"; };
};