#pragma once
#include "TimerUI.h"

class BossAppUI : public TimerUI
{
public:
	void Init()override;
	void Update()override;
	void Draw()override;
	
	void Imgui(const std::string& title = "タイマー");

private:
	Sprite atText;
	Sprite enemyAllKillText;

	Vector3 atPlusPos;
	Vector3 atPlusScale;
	Vector3 enemyAllKillPlusPos;
	Vector3 enemyAllKillPlusScale;
};

