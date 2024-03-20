#pragma once
#include <string>
#include "Easing.h"
#include <vector>
#include <fstream>

struct SpawnOrderOnce
{
	//出現済みか
	bool spawnCompletion = false;

	//出現させたい敵のクラス名
	std::string enemyClassName;

	//出現させるタイミング
	float spawnTiming;

	//出現させる数
	int32_t spawnNum;
};

class SpawnOrderData
{
public:
	//読み込み
	static SpawnOrderData Load(const std::string& filename);
	
	//未定
	//void Save();
public:
	//スポナーの生存時間
	float maxTime;
	std::vector<SpawnOrderOnce> orders;
};

