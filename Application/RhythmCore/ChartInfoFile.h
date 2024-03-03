/*
* @file ChartInfoFile.h
* @brief 楽曲情報ファイル
* 楽曲選択時に必要な情報を記すファイル
* 読み込み, 書き出し機能もここ
*/

#pragma once

//外部ヘッダ
#pragma warning(push, 0)
#pragma warning(disable : 26819)
#include "json.hpp"
#pragma warning(pop)

#include <string>
#include <map>

class ChartInfoFile
{
public:
	std::wstring path;

	std::string displayName;
	std::string composerName;
	std::string charterName;
	std::string displayBPM;
	std::string artworkPath;
	std::string audioPath;
	float previewAudioPosStart;
	float previewAudioPosEnd;

	struct Difficulty {
		int32_t num;
		std::string displayName;
		std::string composerName;
		std::string charterName;
		std::string displayBPM;
		std::string displayLevel;
		std::string chartPath;
		std::string artworkPath;
		std::string previewAudioPath;
		float previewAudioPosStart;
		float previewAudioPosEnd;
	};

	std::unordered_map<std::string, Difficulty> difficulties;

	ChartInfoFile() {}
	ChartInfoFile(std::wstring path) : path(path) {}

	bool Load();
	bool Save();

	static std::string GetDifficultyName(int32_t num);
	static int32_t GetDifficultyNum(std::string name);
};

//JsonWrapper
using json = nlohmann::json;

//Difficulty
void to_json(json& j, const ChartInfoFile::Difficulty& o);
void from_json(const json& j, ChartInfoFile::Difficulty& o);