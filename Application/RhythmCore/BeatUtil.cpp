#include "BeatUtil.h"

void MusicDesc::Init()
{
	changeList.clear();
	for (const std::pair<Beat, float>& pair : bpmChange) {
		if (!Util::Contains(changeList, pair.first)) {
			changeList.push_back(pair.first);
		}
	}
	for (const std::pair<Beat, Meter>& pair : meterChange) {
		if (!Util::Contains(changeList, pair.first)) {
			changeList.push_back(pair.first);
		}
	}

	changeList.sort();
}

float MusicDesc::GetBPM(const Beat& beat) {
	//指定タイミング時点でのBPMを得る
	float bpm = baseBPM;

	for (const std::pair<const Beat, float>& pair : bpmChange) {
		if (pair.first <= beat) {
			bpm = pair.second;
			continue;
		}
		break;
	}

	return bpm;
}

Meter MusicDesc::GetMeter(const Beat& beat) {
	//指定タイミング時点でのBPMを得る
	Meter meter = baseMeter;

	for (const std::pair<const Beat, Meter>& pair : meterChange) {
		if (pair.first <= beat) {
			meter = pair.second;
			continue;
		}
		break;
	}
	return meter;
}

float MusicDesc::GetWholeNoteLength(const Beat& beat)
{
	//指定タイミング時点でのBPMを得る
	float bpm = GetBPM(beat);

	//全音符の長さ(一般にBPMは1分間の四分音符数なのでそれに合わせた計算)
	return ((60 / bpm) * 1000.0f) * 4.0f;
}

float MusicDesc::GetMeasureLength(const int32_t& measure)
{
	float length = 0;
	Beat now = { measure, 0, 1 };

	std::vector<Beat> changes;
	for (std::pair<Beat, float> change : bpmChange) {
		if (ConvertBeatToMiliSeconds(change.first) > ConvertBeatToMiliSeconds({ measure, 0, 1 })
			&& ConvertBeatToMiliSeconds(change.first) < ConvertBeatToMiliSeconds({ measure + 1, 0, 1 })) {
			changes.push_back(change.first);
		}
	}

	Meter meter = GetMeter(now); //指定小節の拍子を得る

	for (Beat b : changes) {
		float whole = GetWholeNoteLength(b); //全音符の長さを得る

		float timeA = (float)now.beat / now.LPB;
		float timeB = (float)b.beat / b.LPB;
		length += (whole / meter.beatLength) * meter.beatAmount * (timeB - timeA);
		now = b;
	}

	float whole = GetWholeNoteLength(now); //全音符の長さを得る
	float time = (float)now.beat / now.LPB;
	length += (whole / meter.beatLength)* meter.beatAmount * (1 - time);
	return length;
}

float MusicDesc::ConvertBeatToMiliSeconds(const Beat& beat)
{
	float beattime = 0;
	Beat now = { 0, 0, 1 };

	for (Beat change : changeList) {
		if (beat < change) {
			break;
		}

		auto cache = cacheMiliSec.find(change);
		if (cache == cacheMiliSec.end()) {
			float whole = GetWholeNoteLength(now);

			Meter meter = GetMeter(now);

			beattime += (whole / meter.beatLength) * meter.beatAmount * (change.measure - now.measure);

			float diff = (float)change.beat / change.LPB - (float)now.beat / now.LPB;

			beattime += whole * diff;
			cacheMiliSec[change] = beattime;
		}
		else {
			beattime = (*cache).second;
		}

		now = change;
	}

	float whole = GetWholeNoteLength(now);

	Meter meter = GetMeter(now);
	
	beattime += (whole / meter.beatLength) * meter.beatAmount * (beat.measure - now.measure);
	
	float diff = (float)beat.beat / beat.LPB - (float)now.beat / now.LPB;
	beattime += whole * diff;
	
	return beattime;
}

Beat MusicDesc::ConvertMiliSecondsToBeat(const float& miliSec)
{
	Beat nowBeat = { 0, 0, baseMeter.beatLength };

	for (Beat change : changeList) {
		if (miliSec < ConvertBeatToMiliSeconds(change)) {
			break;
		}

		nowBeat = change;
	}

	Beat result = nowBeat;

	if (miliSec < 0) {
		result.measure--;
	}

	float whole = GetWholeNoteLength(nowBeat);
	Meter meter = GetMeter(nowBeat);

	result.beat = static_cast<int32_t>(result.beat * (meter.beatLength / static_cast<float>(result.LPB)));
	result.LPB = meter.beatLength;

	float remain = miliSec - ConvertBeatToMiliSeconds(nowBeat);

	int32_t remainMeasure = static_cast<int32_t>(remain / ((whole / meter.beatLength) * meter.beatAmount));
	result.measure += remainMeasure;

	remain = miliSec - ConvertBeatToMiliSeconds(result);

	int32_t beat = static_cast<int32_t>(remain / (whole / meter.beatLength));
	result.beat += beat;

	while (result.beat >= meter.beatAmount) {
		result.measure++;
		result.beat -= meter.beatAmount;
	}

	result.LPB = meter.beatLength;

	return result;
}

void MusicDesc::InitMetronome(const float& miliSec)
{
	metronomeBeat = ConvertMiliSecondsToBeat(miliSec);
	metronomeBeat.beat++;
	while (metronomeBeat.beat >= metronomeBeat.LPB) {
		metronomeBeat.measure++;
		metronomeBeat.beat -= metronomeBeat.LPB;
	}
}

void MusicDesc::PlayMetronome(const float& miliSec)
{
	if (ConvertBeatToMiliSeconds(metronomeBeat) < miliSec) {
		if (metronomeBeat.beat == 0) {
			//PlaySE("Kan");
		}
		//PlaySE("CursorMove");

		Meter meter = GetMeter(metronomeBeat);
		metronomeBeat.beat++;
		while (metronomeBeat.beat >= meter.beatAmount) {
			metronomeBeat.measure++;
			metronomeBeat.beat = 0;
			metronomeBeat.LPB = GetMeter(metronomeBeat).beatLength;
		}
	}
}
