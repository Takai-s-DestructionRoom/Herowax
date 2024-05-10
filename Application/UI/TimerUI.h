#pragma once
#include "Sprite.h"
#include <array>
#include "Easing.h"

/// <summary>
/// 汎用タイマーUI
/// 9:59まで対応
/// </summary>
class ITimerUI
{
public:
	static void LoadResource();

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual ~ITimerUI() {};

	void Imgui(const std::string& title = "タイマー");

	//図る時間を設定
	void SetMaxTime(float maxTime_) { timer.maxTime_ = maxTime_;};

	void Start();
	void ReStart();
	void Stop();

	bool GetRun();
	bool GetEnd();

protected:
	std::array<Sprite, 4> numberSprites;

	std::array<std::string, 11> numberTextures;

	Vector3 basePos;
	Vector3 baseScale;

	float interX = 0;	//間隔

	int32_t minute = 0;
	int32_t tenSecond = 0;
	int32_t oneSecond = 0;

	Easing::EaseTimer timer = 60.f;
};

class TimerUI : public ITimerUI
{
public:
	void Init()override;
	void Update()override;
	void Draw()override;
};