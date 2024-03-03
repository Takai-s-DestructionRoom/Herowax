#include "ChartFile.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <PathUtil.h>

using namespace std;

bool ChartFile::Load()
{
	std::filesystem::path loadPath = PathUtil::ConvertAbsolute(path);
	ifstream ifs(loadPath);
	if (ifs) {
		json j;

		try {
			ifs >> j;

			if (j["audiopath"].is_string()) {
				audiopath = j["audiopath"].get<string>();
				std::wstring wStr = PathUtil::ConvertRelativeFromTo(loadPath, Util::ConvertStringToWString(audiopath)).c_str();
				audiopath = Util::ConvertWStringToString(wStr);
			}
			
			if (!j["audiooffset"].is_null()) {
				audioOffset = j["audiooffset"].get<float>();
			}
			
			music = MusicDesc();

			if (!j["basebpm"].is_null()) {
				music.baseBPM = j["basebpm"].get<float>();
			}

			if (!j["basemeter"].is_null()) {
				music.baseMeter = j["basemeter"].get<Meter>();
			}

			json bpms = j["bpmchange"];
			if (!bpms.is_null()) {
				for (json::iterator itr = bpms.begin(); itr != bpms.end(); itr++) {
					json obj = *itr;
					
					if (!obj["beat"].is_null() && !obj["bpm"].is_null()) {
						music.bpmChange[obj["beat"].get<Beat>()] = obj["bpm"].get<float>();
					}
				}
			}

			json meters = j["meterchange"];
			if (!meters.is_null()) {
				for (json::iterator itr = meters.begin(); itr != meters.end(); itr++) {
					json obj = *itr;

					if (!obj["beat"].is_null() && !obj["meter"].is_null()) {
						music.meterChange[obj["beat"].get<Beat>()] = obj["meter"].get<Meter>();
					}
				}
			}

			music.Init();

			scrollChange.clear();
			json scrolls = j["scrollchange"];
			if (!scrolls.is_null()) {
				for (json::iterator itr = scrolls.begin(); itr != scrolls.end(); itr++) {
					json obj = *itr;

					if (!obj["beat"].is_null() && !obj["scroll"].is_null()) {
						scrollChange[obj["beat"].get<Beat>()] = obj["scroll"].get<float>();
					}
				}
			}

			notes.clear();
			json _notes = j["notes"];
			if (!_notes.is_null()) {
				for (json::iterator itr = _notes.begin(); itr != _notes.end(); itr++) {
					json obj = *itr;

					notes.push_back(obj.get<Note>());
				}
			}

			json _bgName = j["bg"];
			if (!_bgName.is_null()) {
				bgName = _bgName.get<std::string>();
			}

			events.clear();
			json _events = j["event"];
			if (!_events.is_null()) {
				for (json::iterator itr = _events.begin(); itr != _events.end(); itr++) {
					json obj = *itr;

					if (!obj["beat"].is_null() && !obj["name"].is_null()) {
						Event event;

						event.name = obj["name"].get<std::string>();
						if (obj["paramInt"].is_number_integer()) event.paramInt = obj["paramInt"].get<int>();
						if (obj["paramFloat"].is_number_float()) event.paramFloat = obj["paramFloat"].get<float>();
						if (obj["paramBool"].is_boolean()) event.paramBool = obj["paramBool"].get<bool>();
						if (obj["paramString"].is_string()) event.paramString = obj["paramString"].get<std::string>();

						events[obj["beat"].get<Beat>()].push_back(event);
					}
				}
			}
		}
		catch (json::exception e) {
			Util::DebugLog(e.what());
			return false;
		}

		noteCount = 0;
		for (Note& note : notes) {
			switch (note.type) {
			case Note::Types::Tap:
				noteCount++;
				break;
			case Note::Types::Arc:
				Beat arcStartBeat, arcEndBeat;
				Vector3 arcStartPos, arcEndPos;
				float arcStartTime = 0;
				arcStartBeat = note.mainBeat;
				arcStartPos = note.mainPos;
				arcStartTime = music.ConvertBeatToMiliSeconds(arcStartBeat);

				auto itr = note.controlPoints.begin();
				while (true) {
					if (itr != note.controlPoints.end()) {
						arcEndBeat = itr->beat;
						arcEndPos = itr->pos;
					}
					else {
						arcEndBeat = note.subBeat;
						arcEndPos = note.subPos;
					}

					Vector3 diffPos = arcEndPos - arcStartPos;
					if (arcStartBeat == arcEndBeat) {
						noteCount++;
					}

					arcStartBeat = arcEndBeat;
					arcStartPos = arcEndPos;
					arcStartTime = music.ConvertBeatToMiliSeconds(arcStartBeat);
					if (itr != note.controlPoints.end()) {
						itr++;
						continue;
					}
					else {
						break;
					}
				}

				float arcPoint = music.ConvertBeatToMiliSeconds(note.mainBeat);
				while (arcPoint <= music.ConvertBeatToMiliSeconds(note.subBeat)) {
					noteCount++;
					arcPoint += music.ConvertBeatToMiliSeconds({ 0, 1, 8 });
				}
				break;
			}
		}

		return true;
	}
	else {
		Util::DebugLog(path + L" 読み込めませんでした");
		return false;
	}

	return false;
}

bool ChartFile::Save() {

	json j;

	j["audiopath"] = audiopath;
	j["audiooffset"] = audioOffset;
	j["basebpm"] = music.baseBPM;
	j["basemeter"] = music.baseMeter;

	for (std::pair<Beat, float> c : music.bpmChange) {
		json obj;
		obj["beat"] = c.first;
		obj["bpm"] = c.second;
		j["bpmchange"].push_back(obj);
	}

	for (std::pair<Beat, Meter> c : music.meterChange) {
		json obj;
		obj["beat"] = c.first;
		obj["meter"] = c.second;
		j["meterchange"].push_back(obj);
	}

	for (std::pair<Beat, float> c : scrollChange) {
		json obj;
		obj["beat"] = c.first;
		obj["scroll"] = c.second;
		j["scrollchange"].push_back(obj);
	}

	for (Note& n : notes) {
		j["notes"].push_back(n);
	}

	for (auto& ev : events) {
		for (auto& e : ev.second) {
			json obj;
			obj["beat"] = ev.first;
			obj["name"] = e.name;
			obj["paramInt"] = e.paramInt;
			obj["paramFloat"] = e.paramFloat;
			obj["paramBool"] = e.paramBool;
			obj["paramString"] = e.paramString;
			j["event"].push_back(obj);
		}
	}

	j["bg"] = bgName;

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

void to_json(json& j, const Beat& o)
{
	j["measure"] = o.measure;
	j["beat"] = o.beat;
	j["lpb"] = o.LPB;
}

void from_json(const json& j, Beat& o)
{
	if (j.find("measure") != j.end()) {
		o.measure = j.at("measure").get<int32_t>();
	}
	if (j.find("beat") != j.end()) {
		o.beat = j.at("beat").get<int32_t>();
	}
	if (j.find("lpb") != j.end()) {
		o.LPB = j.at("lpb").get<int32_t>();
	}
}

void to_json(json& j, const Meter& o)
{
	j["amount"] = o.beatAmount;
	j["length"] = o.beatLength;
}

void from_json(const json& j, Meter& o)
{
	if (j.find("amount") != j.end()) {
		o.beatAmount = j.at("amount").get<int32_t>();
	}
	if (j.find("length") != j.end()) {
		o.beatLength = j.at("length").get<int32_t>();
	}
}

void to_json(json& j, const Note& o)
{
	j["type"] = static_cast<int32_t>(o.type);
	switch (o.type) {
	case Note::Types::Tap:
		j["beat"] = o.mainBeat;
		j["lane"] = o.lane;
		break;
	case Note::Types::Hold:
		j["beat"] = o.mainBeat;
		j["endbeat"] = o.subBeat;
		j["lane"] = o.lane;
		break;
	case Note::Types::Arc:
		j["beat"] = o.mainBeat;
		j["pos"] = o.mainPos;
		j["endbeat"] = o.subBeat;
		j["endpos"] = o.subPos;
		for (Note::ControlPoint cp : o.controlPoints) {
			j["controlpoints"].push_back(cp);
		}
		break;
	default:
		break;
	}
}

void from_json(const json& j, Note& o)
{
	if (j.find("type") != j.end()) {
		o.type = static_cast<Note::Types>(j.at("type").get<int32_t>());
	}
	else {
		o.type = Note::Types::Tap;
	}

	switch (o.type) {
	case Note::Types::Tap:
		if (j.find("beat") != j.end()) {
			o.mainBeat = j.at("beat").get<Beat>();
		}
		if (j.find("lane") != j.end()) {
			o.lane = j.at("lane").get<int32_t>();
		}
		break;
	case Note::Types::Hold:
		if (j.find("beat") != j.end()) {
			o.mainBeat = j.at("beat").get<Beat>();
		}
		if (j.find("lane") != j.end()) {
			o.lane = j.at("lane").get<int32_t>();
		}
		if (j.find("endbeat") != j.end()) {
			o.subBeat = j.at("endbeat").get<Beat>();
		}
		else {
			o.subBeat = o.mainBeat;
		}
		break;
	case Note::Types::Arc:
		if (j.find("beat") != j.end()) {
			o.mainBeat = j.at("beat").get<Beat>();
		}
		if (j.find("pos") != j.end()) {
			o.mainPos = j.at("pos").get<Vector3>();
		}
		if (j.find("endbeat") != j.end()) {
			o.subBeat = j.at("endbeat").get<Beat>();
		}
		else {
			o.subBeat = o.mainBeat;
		}
		if (j.find("endpos") != j.end()) {
			o.subPos = j.at("endpos").get<Vector3>();
		}
		else {
			o.subPos = o.mainPos;
		}
		if (j.find("controlpoints") != j.end()) {
			json controlPoints = j["controlpoints"];
			for (json::iterator itr = controlPoints.begin(); itr != controlPoints.end(); itr++) {
				json obj = *itr;

				o.controlPoints.push_back(obj.get<Note::ControlPoint>());
			}

			std::stable_sort(o.controlPoints.begin(), o.controlPoints.end(), [](Note::ControlPoint const& lhs, Note::ControlPoint const& rhs) {
				return lhs.beat < rhs.beat;
			});
		}
		break;
	default:
		break;
	}
}

void to_json(json& j, const Note::ControlPoint& o)
{
	j["beat"] = o.beat;
	j["pos"] = o.pos;
}

void from_json(const json& j, Note::ControlPoint& o)
{
	if (j.find("beat") != j.end()) {
		o.beat = j.at("beat").get<Beat>();
	}
	if (j.find("pos") != j.end()) {
		o.pos = j.at("pos").get<Vector3>();
	}
}

void to_json(json& j, const Vector3& o)
{
	j["x"] = o.x;
	j["y"] = o.y;
	if (o.z != 0) j["z"] = o.z;
}

void from_json(const json& j, Vector3& o)
{
	if (j.find("x") != j.end()) {
		o.x = j.at("x");
	}
	if (j.find("y") != j.end()) {
		o.y = j.at("y");
	}
	if (j.find("z") != j.end()) {
		o.z = j.at("z");
	}
}
