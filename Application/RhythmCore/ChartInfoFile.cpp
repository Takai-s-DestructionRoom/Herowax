#include "ChartInfoFile.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <PathUtil.h>
#include <Util.h>

using namespace std;

bool ChartInfoFile::Load()
{
	std::filesystem::path loadPath = PathUtil::ConvertAbsolute(path);
	ifstream ifs(loadPath);
	if (ifs) {
		json j;

		try {
			ifs >> j;

			if (j["artworkpath"].is_string()) {
				artworkPath = j["artworkpath"].get<string>();
				std::wstring wStr = PathUtil::ConvertRelativeFromTo(loadPath, Util::ConvertStringToWString(artworkPath)).c_str();
				artworkPath = Util::ConvertWStringToString(wStr);
			}

			if (j["audiopath"].is_string()) {
				audioPath = j["audiopath"].get<string>();
				std::wstring wStr = PathUtil::ConvertRelativeFromTo(loadPath, Util::ConvertStringToWString(audioPath)).c_str();
				audioPath = Util::ConvertWStringToString(wStr);
			}

			if (j["previewaudiostart"].is_number_float() && j["previewaudioend"].is_number_float()) {
				previewAudioPosStart = j["previewaudiostart"].get<float>();
				previewAudioPosEnd = j["previewaudioend"].get<float>();
			}

			if (j["displayname"].is_string()) {
				displayName = j["displayname"].get<string>();
			}

			if (j["displayname"].is_string()) {
				displayName = j["displayname"].get<string>();
			}

			if (j["composer"].is_string()) {
				composerName = j["composer"].get<string>();
			}

			if (j["charter"].is_string()) {
				charterName = j["charter"].get<string>();
			}

			if (j["bpm"].is_string()) {
				displayBPM = j["bpm"].get<string>();
			}

			difficulties.clear();
			if (!j["difficulty"].is_null()) {
				for (auto itr = j["difficulty"].begin(); itr != j["difficulty"].end(); itr++) {
					if (itr->is_object()) {
						Difficulty& diff = difficulties[itr.key()];
						diff = itr->get<Difficulty>();
						diff.num = GetDifficultyNum(itr.key());
					}
				}
			}
		}
		catch (json::exception e) {
			Util::DebugLog(e.what());
			return false;
		}
		return true;
	}
	else {
		Util::DebugLog(path + L" 読み込めませんでした");
		return false;
	}

	return false;
}

bool ChartInfoFile::Save() {

	json j;

	j["displayname"] = displayName;
	j["composer"] = composerName;
	j["charter"] = charterName;
	j["bpm"] = displayBPM;
	j["artworkpath"] = artworkPath;
	j["audiopath"] = audioPath;

	json diffs = j["difficulty"];

	for (auto& pair : difficulties) {
		Difficulty& diff = pair.second;
		json d = diffs[pair.first];
		d["composer"] = diff.composerName;
		d["charter"] = diff.charterName;
		d["bpm"] = diff.displayBPM;
		d["level"] = diff.displayLevel;
		d["chartpath"] = diff.chartPath;
		d["audiopath"] = diff.previewAudioPath;
	}

	ofstream ofs(PathUtil::ConvertAbsolute(path));
	if (ofs) {
		ofs << j.dump(4);
		return true;
	}
	else {
		Util::DebugLog(path + L"書き出せませんでした");
		return false;
	}
	return false;
}

std::string ChartInfoFile::GetDifficultyName(int32_t num)
{
	switch (num) {
	case 1:
		return "easy";
	case 2:
		return "normal";
	case 3:
		return "hard";
	default:
		return "";
	}
}

int32_t ChartInfoFile::GetDifficultyNum(std::string name)
{
	if (name == "easy") return 1;
	else if (name == "normal") return 2;
	else if (name == "hard") return 3;
	return 0;
}

void to_json(json& j, const ChartInfoFile::Difficulty& o)
{
	j["displayname"] = o.displayName;
	j["composer"] = o.composerName;
	j["charter"] = o.charterName;
	j["bpm"] = o.displayBPM;
	j["level"] = o.displayLevel;
	j["chartpath"] = o.chartPath;
	j["artworkpath"] = o.artworkPath;
	j["audiopath"] = o.previewAudioPath;
	j["previewaudiostart"] = o.previewAudioPosStart;
	j["previewaudioend"] = o.previewAudioPosEnd;
}

void from_json(const json& j, ChartInfoFile::Difficulty& o)
{
	if (j.find("displayname") != j.end() && j["displayname"].is_string()) {
		o.displayName = j["displayname"].get<std::string>();
	}
	if (j.find("composer") != j.end() && j["composer"].is_string()) {
		o.composerName = j["composer"].get<std::string>();
	}
	if (j.find("charter") != j.end() && j["charter"].is_string()) {
		o.charterName = j["charter"].get<std::string>();
	}
	if (j.find("bpm") != j.end() && j["bpm"].is_string()) {
		o.displayBPM = j["bpm"].get<std::string>();
	}
	if (j.find("level") != j.end() && j["level"].is_string()) {
		o.displayLevel = j["level"].get<std::string>();
	}
	if (j.find("chartpath") != j.end() && j["chartpath"].is_string()) {
		o.chartPath = j["chartpath"].get<std::string>();
	}
	if (j.find("artworkpath") != j.end() && j["artworkpath"].is_string()) {
		o.artworkPath = j["artworkpath"].get<std::string>();
	}
	if (j.find("audiopath") != j.end() && j["audiopath"].is_string()) {
		o.previewAudioPath = j["audiopath"].get<std::string>();
	}
	if (j.find("previewaudiostart") != j.end() && j.find("previewaudioend") != j.end()
		&& j["previewaudiostart"].is_number_float() && j["previewaudioend"].is_number_float()) {
		o.previewAudioPosStart = j["previewaudiostart"].get<float>();
		o.previewAudioPosEnd = j["previewaudioend"].get<float>();
	}
}
