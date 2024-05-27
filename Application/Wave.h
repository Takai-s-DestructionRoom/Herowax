#pragma once
#include <vector>
#include <string>
#include <Easing.h>
#include <LevelData.h>

class Wave
{
public:
	//開始の猶予時間
	float startPostponement = 0.0f;
	//読み込むレベルデータのハンドル
	std::string handle = "";
};

class WaveManager
{
public:
	static WaveManager* Get();

	//レベルデータを先に読み込んでおく
	void LoadLevelData();

	void Init();
	void Update();

	void Imgui();

	//ウェーブを進める関数
	void NextWave();
	void MoveWave(int32_t moveNum);

private:
	WaveManager() {};
	~WaveManager() {};
private:
	int32_t debugNum = 1;

	std::vector<std::string> waves;

	int32_t waveNum = -1;
	int32_t MAX_NUM = 8;

	//倒す敵の数(読み込み時に敵を数える)
	int32_t enemyNum = 0;

	Easing::EaseTimer waitTimer = 1.0f;

	bool zeroFlag = false;
	bool bossApp = false;
};

