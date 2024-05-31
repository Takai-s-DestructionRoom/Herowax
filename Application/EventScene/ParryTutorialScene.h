#pragma once
#include "IEventScene.h"
#include "Vector2.h"

class ParryTutorialScene : public IEventScene
{
private:
	Vector2 buttonUIPos{};

public:
	ParryTutorialScene();
	~ParryTutorialScene();

	void Init(const Vector3 target) override;
	void Update() override;
	void Draw() override;

	static std::string GetEventCallStr() { return "ParryTutorialScene"; };
};

