/*
* @file ChartFile.h
* @brief 譜面ファイルクラス
* ゲーム中の楽曲や譜面そのものを記すファイル
* 読み込み, 書き出し機能もここ
*/

#pragma once

//外部ヘッダ
#pragma warning(push, 0)
#pragma warning(disable : 26819)
#include "json.hpp"
#pragma warning(pop)

#include <BeatUtil.h>
#include <Note.h>
#include <string>
#include "EventSystem.h"

class ChartFile
{
public:
	std::wstring path;

	std::string audiopath;
	float audioOffset = 0;

	MusicDesc music;

	std::map<Beat, float> scrollChange;

	std::list<Note> notes;
	int32_t noteCount = 0;

	std::string bgName;
	std::map<Beat, std::vector<Event>> events;

	ChartFile() {}
	ChartFile(std::wstring path) : path(path) {}

	bool Load();
	bool Save();
};

//JsonWrapper
using json = nlohmann::json;

//Vector3
void to_json(json& j, const Vector3& o);
void from_json(const json& j, Vector3& o);

//Beat
void to_json(json& j, const Beat& o);
void from_json(const json& j, Beat& o);

//Meter
void to_json(json& j, const Meter& o);
void from_json(const json& j, Meter& o);

//Note
void to_json(json& j, const Note& o);
void from_json(const json& j, Note& o);

//Note::ControlPoint
void to_json(json& j, const Note::ControlPoint& o);
void from_json(const json& j, Note::ControlPoint& o);
