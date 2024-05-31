#pragma once
#include "IEventScene.h"
#include "Vector2.h"
#include "CircleGauge.h"

class MoveTutorialScene : public IEventScene
{
private:
	Vector2 tutorialUIPos{};

	CircleGauge moveCountGauge;		//動かしたらゲージが伸びて
	CircleGauge cameraCountGauge;	

	Sprite moveOk;		//okって出したい
	Sprite cameraOk;	

public:
	MoveTutorialScene();
	~MoveTutorialScene();

	void Init(const Vector3 target) override;
	void Update() override;
	void Draw() override;

	static std::string GetEventCallStr() { return "MoveTutorialScene"; };
};