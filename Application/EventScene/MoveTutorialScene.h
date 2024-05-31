#pragma once
#include "IEventScene.h"
#include "Vector2.h"

class MoveTutorialScene : public IEventScene
{
private:
	Vector2 tutorialUIPos{};

	Easing::EaseTimer moveCountTimer;			//移動時間測る用
	Easing::EaseTimer cameraMoveCountTimer;		//カメラ移動時間測る用

public:
	MoveTutorialScene();
	~MoveTutorialScene();

	void Init(const Vector3 target) override;
	void Update() override;
	void Draw() override;

	static std::string GetEventCallStr() { return "MoveTutorialScene"; };
};