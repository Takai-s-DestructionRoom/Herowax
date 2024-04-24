#pragma once
#include "IEventScene.h"
#include <array>

//ボスの撃破演出シーン
class BossDeadScene : public IEventScene
{
public:
	Easing::EaseTimer waitTimer;
	std::array<Vector3, 3>cameraPos;					//それぞれのカメラ座標
	std::array<Easing::EaseTimer, 3>cameraChangeTimer;	//カメラが切り替わるまでの時間

public:
	BossDeadScene();
	~BossDeadScene();

	void Init(const Vector3 target) override;
	void Update() override;
	void Draw() override;

	static std::string GetEventCallStr() {return "BossDeadScene";};
};