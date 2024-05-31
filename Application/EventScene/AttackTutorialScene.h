#pragma once
#include "IEventScene.h"
#include "Vector2.h"

class AttackTutorialScene : public IEventScene
{
private:
	Vector2 buttonUIPos{};

public:
	AttackTutorialScene();
	~AttackTutorialScene();

	void Init(const Vector3 target) override;
	void Update() override;
	void Draw() override;

	static std::string GetEventCallStr() { return "AttackTutorialScene"; };
};