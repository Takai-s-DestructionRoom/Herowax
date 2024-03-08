#pragma once
#include <string>
#include <Vector3.h>
#include <vector>
#include <unordered_map>
#include "LevelData.h"

//外部のライブラリで出た警告は無視
#pragma warning (push)
#pragma warning ( disable : 26800)
#include <json.hpp>
#pragma warning (pop)

/*! LevelLoader
	blenderアドオンから出力したjsonファイルからステージ情報を読み取り、LevelDataに保存するクラス
*/
class LevelLoader
{
public:
	void Load(const std::string &filename,const std::string& handle);

	LevelData* GetData(const std::string& handle);
	
	std::unordered_map<std::string, LevelData> GetDataMap() {
		return LevelDatas;
	}

	static LevelLoader* Get()
	{
		static LevelLoader instance;
		return &instance;
	}

private:
	void ObjectLoad(LevelData& levelData, nlohmann::json& object);

	int32_t mReservenum = 0;

	LevelLoader(){};
	~LevelLoader(){};

	std::unordered_map<std::string, LevelData> LevelDatas;
};