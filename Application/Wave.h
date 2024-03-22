#pragma once
#include <vector>
#include <string>
#include <Easing.h>

class Wave
{
public:
	//使用するレベルデータたち
	std::vector<std::string> levelFiles;

	int32_t nowLevel;	//現在参照しているレベル

	//開始の猶予時間
	float startPostponement;

	//終了の猶予時間
	float endPostponement;

	//合計時間
	Easing::EaseTimer waveTimer;

	//遷移する時間
	Easing::EaseTimer transitionTimer;

private:
	float spawnerMaxTime = 0;

	bool isStartPost = false;
	bool isEndPost = false;
	bool isEndMain = false;

public:
	Wave();

	void Load();

	void Restart();
	void SetWaveTime();
	
	void Update();

	bool GetStartPostponement();
	bool GetEndPostponement();
	bool GetEndMainGame(float time);
};

