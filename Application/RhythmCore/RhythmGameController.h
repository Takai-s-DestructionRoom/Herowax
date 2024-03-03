/*
* @file RhythmGameController.h
* @brief リズムゲームを制御するクラス
* ゲームシーンの殆どはこれが制御している
*/

#pragma once
#include "BeatUtil.h"
#include "Note.h"
#include "ChartFile.h"
#include <RWindow.h>
#include <ModelObj.h>
#include <RAudio.h>
#include <ArcNoteObj.h>

class RhythmGameController
{
public:
	ChartFile chart;

	bool playing = false;
	bool startedMusicStream = false;
	bool finished = false;
	bool autoplay = false;

	AudioHandle audioHandle;
	MusicDesc music;

	float scrollSpeed = 9;
	std::map<Beat, float> scrollChange;
	std::map<Beat, float> cacheScrollPos;

	std::map<Beat, std::vector<Event>> events;
	std::map<Beat, std::vector<Event>> remainEvents;

	std::list<Note> notes;
	std::list<Note> remainNotes;

	std::vector<ModelObj> noteModelObjs;
	std::vector<ArcNoteObj> arcNoteObjs;
	std::vector<ModelObj> lineModelObjs;

	int32_t score = 0;
	int32_t combo = 0;
	int32_t maxCombo = 0;
	int32_t countJudgePerfect = 0;
	int32_t countJudgeHit = 0;
	int32_t countJudgeMiss = 0;

	float time = -3000;
	float offsetStream = 0;

	float nowPosY = 0;

	RhythmGameController() {}

	float GetScroll(float miliSec);
	float GetPosition(float miliSec);

	void DrawMeasureLine();
	void Reset();
	void Init();
	void Update();

	void Load();
	void Save();

	static constexpr float posJudgeLine = 0;
	static constexpr float laneCenter = 0;
	static constexpr float laneWidth = 4;
	static constexpr float scrollBase = 0.01f;

	static constexpr float judgeUltimatePerfect = 40;
	static constexpr float judgePerfect = 80;
	static constexpr float judgeHit = 100;

private:
	bool CheckArcInput(Beat& arcStartBeat, Beat& arcEndBeat, Vector3& arcStartPos, Vector3& arcEndPos);
	void JudgeRightAngleArc(Note& note);
	Vector3 GetNowArcPos(Note& note);
};

