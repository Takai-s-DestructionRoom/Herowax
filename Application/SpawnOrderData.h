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
	std::string enemyClassName = "";

	//出現させるタイミング
	float spawnTiming = 0.0f;

	//出現させる数
	int32_t spawnNum = 0;
};

class SpawnOrderData
{
public:
	//スポナーの生存時間
	float maxTime = 0.0f;
	//スポナー本体の出現タイミング
	float startTiming = 0.0f;

	std::vector<SpawnOrderOnce> orders;
};

namespace enemyHandles {
	extern std::string enemy;
	extern std::string bombsolider;
}

class SpawnDataLoader
{
public:
	//オーダーを作るImgui
	static void OrderCreateGUI();

	//読み込み
	static SpawnOrderData Load(const std::string& filename);

	//書き出し
	static void Save(const SpawnOrderData& saveOrder, const std::string& saveFileName_);

private:
	//読み込みで使うデータたち
	static std::ofstream writing_file;
	static std::string saveFileName;
	static SpawnOrderData saveOrderData;
	static SpawnOrderOnce once;
};