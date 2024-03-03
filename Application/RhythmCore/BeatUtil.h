/*
* @file BeatUtil.h
* @brief BPMや拍子からの変換等を提供するクラス群
*/

#pragma once
#include "Util.h"
#include <map>
#include <list>

struct Meter {
	int32_t beatAmount = 4; //1小節に入る拍の数
	int32_t beatLength = 4; //1拍の長さ
};

struct Beat {
	int32_t measure = 0; //小節番号
	int32_t beat = 0; //拍数
	int32_t LPB = 4; //1拍の長さ

	bool operator==(const Beat& a) const {
		return measure == a.measure && (float)beat / LPB == (float)a.beat / a.LPB;
	}

	bool operator<(const Beat& a) const {
		if (this->measure < a.measure) {
			return true;
		}
		if (this->measure == a.measure
			&& (float)this->beat / this->LPB < (float)a.beat / a.LPB) {
			return true;
		}

		return false;
	}

	bool operator<=(const Beat& a) const {
		if (this->measure < a.measure) {
			return true;
		}
		if (this->measure == a.measure
			&& (float)this->beat / this->LPB <= (float)a.beat / a.LPB) {
			return true;
		}

		return false;
	}
};

class MusicDesc {
public:
	float baseBPM;
	std::map<Beat, float> bpmChange;
	Meter baseMeter;
	std::map<Beat, Meter> meterChange;
	std::list<Beat> changeList;
	std::map<Beat, float> cacheMiliSec;

	Beat metronomeBeat;

	MusicDesc()
		: baseBPM(120), bpmChange(std::map<Beat, float>{}), baseMeter({ 4, 4 }), meterChange(std::map<Beat, Meter>{}) {}

	MusicDesc(float baseBPM, std::map<Beat, float> bpmChange, Meter baseMeter, std::map<Beat, Meter> meterChange)
		: baseBPM(baseBPM), bpmChange(bpmChange), baseMeter(baseMeter), meterChange(meterChange) {
		Init();
	}

	void Init();

	//指定地点のBPMを得る
	float GetBPM(const Beat& beat = { 0, 0, 1 });

	//指定地点の拍子を得る
	Meter GetMeter(const Beat& beat = { 0, 0, 1 });

	//全音符の長さを得る
	float GetWholeNoteLength(const Beat& beat = {0, 0, 1});

	//1小節の長さを得る
	float GetMeasureLength(const int32_t& measure = 0);

	//拍数からミリ秒に変換する
	float ConvertBeatToMiliSeconds(const Beat& beat);

	//ミリ秒から拍数に変換する
	Beat ConvertMiliSecondsToBeat(const float& miliSec);

	//メトロノーム機能を初期化する
	void InitMetronome(const float& miliSec);

	//メトロノームを再生する
	void PlayMetronome(const float& miliSec);
};
