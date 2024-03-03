#include "RhythmGameController.h"
#include "TimeManager.h"
#include <SimpleDrawer.h>
#include <RInput.h>
#include <RImGui.h>
#include <Camera.h>
#include <ParticleExplode.h>
#include <ParticleSprite.h>
#include <PathUtil.h>
#include <ParticleSprite3D.h>

float RhythmGameController::GetScroll(float miliSec)
{
	std::pair<Beat, float> nowScroll({ 0, 0, 1 }, 1.0f);

	for (std::pair<Beat, float> change : scrollChange) {
		if (miliSec < music.ConvertBeatToMiliSeconds(change.first)) {
			break;
		}
		
		nowScroll = change;
	}

	return nowScroll.second;
}

float RhythmGameController::GetPosition(float miliSec)
{
	float base = scrollBase * scrollSpeed;
	float pos = 0;

	std::pair<Beat, float> nowScroll({ 0, 0, 1 }, 1.0f);

	for (std::pair<Beat, float> change : scrollChange) {
		if (miliSec < music.ConvertBeatToMiliSeconds(change.first)) {
			break;
		}

		auto cache = cacheScrollPos.find(change.first);
		if (cache == cacheScrollPos.end()) {
			pos += (base * nowScroll.second) * (music.ConvertBeatToMiliSeconds(change.first) - music.ConvertBeatToMiliSeconds(nowScroll.first));
			cacheScrollPos[change.first] = pos;
		}
		else {
			pos = (*cache).second;
		}

		nowScroll = change;
	}

	pos += (base * nowScroll.second) * (miliSec - music.ConvertBeatToMiliSeconds(nowScroll.first));
	return pos;
}

void RhythmGameController::DrawMeasureLine()
{
	int32_t lineObjIndex = 0;
	Beat beat = music.ConvertMiliSecondsToBeat(time);
	for (int32_t measure = beat.measure; ;) {
		float posY = posJudgeLine - (GetPosition(music.ConvertBeatToMiliSeconds({ measure, 0, 1 })) - nowPosY);

		if (posY < -80 || posY > 50) {
			break;
		}

		if (abs(beat.measure - measure) > 1000) {
			Util::DebugLog("あぶない");
			break;
		}

		if (lineObjIndex == lineModelObjs.size()) {
			lineModelObjs.emplace_back("Cube");
		}
		ModelObj& obj = lineModelObjs[lineObjIndex];
		obj.mTransform.position = { 0, -0.035f, -posY };
		obj.mTransform.scale = { 16.0f, 0.1f, 0.1f };
		obj.mTransform.UpdateMatrix();
		obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
		obj.Draw();
		lineObjIndex++;
		measure--;
	}
	for (int32_t measure = beat.measure + 1; ;) {
		float posY = posJudgeLine - (GetPosition(music.ConvertBeatToMiliSeconds({ measure, 0, 1 })) - nowPosY);

		if (posY < -80 || posY > 50) {
			break;
		}

		if (abs(beat.measure - measure) > 1000) {
			Util::DebugLog("あぶない");
			break;
		}

		if (lineObjIndex == lineModelObjs.size()) {
			lineModelObjs.emplace_back("Cube");
		}
		ModelObj& obj = lineModelObjs[lineObjIndex];
		obj.mTransform.position = { 0, -0.035f, -posY };
		obj.mTransform.scale = { 16.0f, 0.1f, 0.1f };
		obj.mTransform.UpdateMatrix();
		obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
		obj.Draw();
		lineObjIndex++;
		measure++;
	}
	for (std::pair<Beat, float> change : scrollChange) {
		float posY = posJudgeLine - (GetPosition(music.ConvertBeatToMiliSeconds(change.first)) - nowPosY);

		if (posY < -80 || posY > 50) {
			continue;
		}

		if (lineObjIndex == lineModelObjs.size()) {
			lineModelObjs.emplace_back("Cube");
		}
		ModelObj& obj = lineModelObjs[lineObjIndex];
		obj.mTransform.position = { 0, -0.035f, -posY };
		obj.mTransform.scale = { 16.0f, 0.1f, 0.1f };
		obj.mTransform.UpdateMatrix();
		obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
		obj.Draw();
		lineObjIndex++;
	}
}

void RhythmGameController::Reset()
{
	playing = false;
	startedMusicStream = false;

	audioHandle = "";
	music = MusicDesc();

	scrollChange.clear();
	cacheScrollPos.clear();

	notes.clear();
	remainNotes.clear();

	countJudgePerfect = 0;
	countJudgeHit = 0;
	countJudgeMiss = 0;

	time = -3000;
	offsetStream = 0;

	nowPosY = 0;
}

void RhythmGameController::Init()
{
	if (noteModelObjs.empty()) {
		Model::Load("./Resources/Model/Note/note.obj", "Note");
		for (int32_t i = 0; i < 100; i++) {
			noteModelObjs.emplace_back("Note");
		}
	}

	if (arcNoteObjs.empty()) {
		for (int32_t i = 0; i < 100; i++) {
			arcNoteObjs.emplace_back();
		}
	}

	if (lineModelObjs.empty()) {
		Model::Load("./Resources/Model/cube.obj", "Cube");
		for (int32_t i = 0; i < 100; i++) {
			lineModelObjs.emplace_back("Cube");
		}
	}

	remainNotes.clear();
	for (Note& note : notes) {
		remainNotes.push_back(note);
	}

	remainNotes.sort();

	remainEvents.clear();
	for (auto& e : events) {
		remainEvents[e.first] = e.second;
	}
}

void RhythmGameController::Update()
{
	if (Util::debugBool) {
		if (RInput::GetKey(DIK_LSHIFT) || RInput::GetKey(DIK_RSHIFT)) {
			if (RInput::GetKeyDown(DIK_LEFT)) {
				scrollSpeed -= 1.0f;
			}

			if (RInput::GetKeyDown(DIK_RIGHT)) {
				scrollSpeed += 1.0f;
			}
		}
		else {
			if (RInput::GetKeyDown(DIK_LEFT)) {
				scrollSpeed -= 0.1f;
			}

			if (RInput::GetKeyDown(DIK_RIGHT)) {
				scrollSpeed += 0.1f;
			}
		}
		RWindow::SetMouseLock(false);
	}
	else {
		
		RWindow::SetMouseLock(true);
	}

	if (Util::debugBool) {
		Beat beat = music.ConvertMiliSecondsToBeat(time);

		ImGui::SetNextWindowSize({ 400, 400 });
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 860, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("RhythmGameController", NULL, window_flags);

		if (ImGui::Button("Reset")) {
			playing = false;
			RAudio::Stop(audioHandle);
			RAudio::SetPlayRange(audioHandle, 0, 0);
			startedMusicStream = false;
			chart.Load();
			Load();
			Init();
		}

		ImGui::Separator();

		ImGui::DragFloat("Time", &time);
		ImGui::Text(Util::StringFormat("BPM:%.2f", music.GetBPM(beat)).c_str());
		ImGui::Text(Util::StringFormat("Meter:%d/%d", music.GetMeter(beat).beatAmount, music.GetMeter(beat).beatLength).c_str());
		ImGui::Text(Util::StringFormat("Beat:%d:%d(%d)", beat.measure, beat.beat + 1, beat.LPB).c_str());
		if (ImGui::Button("+1M##Beat")) {
			playing = false;
			RAudio::Stop(audioHandle);
			time += music.GetMeasureLength(beat.measure);
			if (startedMusicStream) {
				RAudio::SetPlayRange(audioHandle, (time - offsetStream) / 1000.0f, 0);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("+1B##Beat")) {
			playing = false;
			RAudio::Stop(audioHandle);
			time += music.GetMeasureLength(beat.measure) / beat.LPB;
			if (startedMusicStream) {
				RAudio::SetPlayRange(audioHandle, (time - offsetStream) / 1000.0f, 0);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("-1B##Beat")) {
			playing = false;
			RAudio::Stop(audioHandle);
			time -= music.GetMeasureLength(beat.measure) / beat.LPB;
			if (startedMusicStream) {
				RAudio::SetPlayRange(audioHandle, (time - offsetStream) / 1000.0f, 0);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("-1M##Beat")) {
			playing = false;
			RAudio::Stop(audioHandle);
			time -= music.GetMeasureLength(beat.measure);
			if (startedMusicStream) {
				RAudio::SetPlayRange(audioHandle, (time - offsetStream) / 1000.0f, 0);
			}
		}
		ImGui::Text(Util::StringFormat("Scroll:%.1f * %.2f", scrollSpeed, GetScroll(time)).c_str());
		ImGui::Spacing();
		ImGui::Text(Util::StringFormat("Score:%d", score).c_str());
		ImGui::Text(Util::StringFormat("Perfect:%d", countJudgePerfect).c_str());
		ImGui::Text(Util::StringFormat("Hit:%d", countJudgeHit).c_str());
		ImGui::Text(Util::StringFormat("Hoge:%d", countJudgeMiss).c_str());
		ImGui::Text(Util::StringFormat("(RemainNotes:%d)", remainNotes.size()).c_str());
		ImGui::Text(Util::StringFormat("(NotesCount:%d)", chart.noteCount).c_str());

		ImGui::Separator();

		ImGui::Checkbox("AutoPlay", &autoplay);
		if (ImGui::Button("Play")) {
			for (auto itr = remainNotes.begin(); itr != remainNotes.end();) {
				Note& note = *itr;
				if (music.ConvertBeatToMiliSeconds(note.mainBeat) < time) {
					itr = remainNotes.erase(itr);
					continue;
				}
				itr++;
			}

			for (auto itr = remainEvents.begin(); itr != remainEvents.end();) {
				auto& e = *itr;
				if (music.ConvertBeatToMiliSeconds(e.first) < time) {
					itr = remainEvents.erase(itr);
					continue;
				}
				itr++;
			}
			playing = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop")) playing = false;

		ImGui::End();
	}

	if (playing) {
		if (!audioHandle.empty() && RAudio::IsPlaying(audioHandle)) {
			time = offsetStream + RAudio::GetCurrentPosition(audioHandle) * 1000.0f;
		}
		else {
			time += TimeManager::deltaMiliTime;
			if (!audioHandle.empty() && !startedMusicStream && time >= offsetStream) {
				startedMusicStream = true;
				RAudio::SetPlayRange(audioHandle, (time - offsetStream) / 1000.0f, 0);
				RAudio::Play(audioHandle, 0.5f);
			}
		}

		for (auto itr = remainEvents.begin(); itr != remainEvents.end();) {
			if (music.ConvertBeatToMiliSeconds(itr->first) < time) {
				for (Event& e : itr->second) {
					EventSystem::TriggerEvent(e);
				}
				itr = remainEvents.erase(itr);
				continue;
			}
			itr++;
		}

		if ((remainNotes.empty() && !RAudio::IsPlaying(audioHandle))
			|| EventSystem::HasTriggeredEvent("finish")) {
			RAudio::Stop(audioHandle);
			finished = true;
		}
	}
	else {
		if (!audioHandle.empty()) {
			RAudio::Stop(audioHandle);
			startedMusicStream = false;
		}
	}

	nowPosY = GetPosition(time);

	uint8_t laneKey[4] = { DIK_F, DIK_G, DIK_H, DIK_J };

	Beat beat = music.ConvertMiliSecondsToBeat(time);

	bool existNearby[4] = {};
	Note nearbyNotes[4] = {};

	int32_t noteObjIndex = 0;
	int32_t arcObjIndex = 0;
	//一番近いノーツを見つけます
	for (Note& note : remainNotes) {
		if (note.type == Note::Types::Arc) {
			continue;
		}
		if (existNearby[note.lane]) {
			float diffA = abs(time - music.ConvertBeatToMiliSeconds(nearbyNotes[note.lane].mainBeat));
			float diffB = abs(time - music.ConvertBeatToMiliSeconds(note.mainBeat));
			if (diffB < diffA) {
				nearbyNotes[note.lane] = note;
			}
		}
		else {
			nearbyNotes[note.lane] = note;
			existNearby[note.lane] = true;
		}
	}

	std::list<Note> removeNotes;
	for (Note& note : remainNotes) {
		//ノーツの位置計算
		float posX = laneCenter + (-laneWidth * 2 + laneWidth * note.lane + laneWidth / 2);
		if (!note.posCalced) {
			if (note.type == Note::Types::Tap || note.type == Note::Types::Hold) {
				note.mainPos.x = laneCenter + (-laneWidth * 2 + laneWidth * note.lane + laneWidth / 2);
				note.mainPos.y = 0;
				note.subPos.x = note.mainPos.x;
				note.subPos.y = note.mainPos.y;
			}

			note.mainPos.z = GetPosition(music.ConvertBeatToMiliSeconds(note.mainBeat));
			if (note.type == Note::Types::Hold) {
				note.subPos.z = GetPosition(music.ConvertBeatToMiliSeconds(note.subBeat));
			}

			if (note.type == Note::Types::Arc) {
				for (auto& cp : note.controlPoints) {
					cp.pos.z = GetPosition(music.ConvertBeatToMiliSeconds(cp.beat));
					cp.posCalced = true;
				}
				note.subPos.z = GetPosition(music.ConvertBeatToMiliSeconds(note.subBeat));
			}

			note.posCalced = true;
		}
		float posZ = posJudgeLine - (note.mainPos.z - nowPosY);

		//入力判定
		if (playing) {
			if (note.type == Note::Types::Tap) {
				if (note.judgeFlag && note.judgeDiff <= judgePerfect && (music.ConvertBeatToMiliSeconds(note.mainBeat) - time) <= 0) {
					RAudio::Play("JudgePerfect");
					ParticleSprite::Spawn({posX, 0, 0}, "judgeText", RRect(9, 250, 10, 55), {0.5f, 0.5f}, {1, 1, 1, 1}, 90, 50, 0, 0.5f, 0.5f, 0.2f);
					for (int32_t i = 0; i < 36; i++) {
						Vector3 speed = { Util::GetRand(-5.0f, 5.0f), Util::GetRand(-5.0f, 5.0f), 0 };
						//ParticleSprite3D::Spawn({ posX, 0, 0 }, "ParticleCircle", RRect(0, 64, 0, 64), { 0.5f, 0.5f }, { 1, 1, 1, 1 }, Util::GetRand(0.0f, 360.0f), Util::GetRand(0.01f, 0.05f), speed * 5, speed, { 0, 0, 0 }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, {0.0f, 0.0f}, 0.1f, 0.1f, 0.2f, false);
						Vector3 speedB = { 5.0f * cos(Util::AngleToRadian((360.0f / 36) + (360.0f / 36 * i))), 5.0f * sin(Util::AngleToRadian((360.0f / 12) + (360.0f / 12 * i))), 0};
						ParticleSprite3D::Spawn({ posX, 0, 0 }, "ParticleCircle", RRect(0, 64, 0, 64), { 0.5f, 0.5f }, { Util::GetRand(0.1f, 1.0f), Util::GetRand(0.1f, 1.0f), Util::GetRand(0.1f, 1.0f), 1 }, 0, 0, speedB * 5, speedB, { 0, 0, 0 }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f }, 0.1f, 0.1f, 0.2f, false);
						ParticleSprite3D::Spawn({ posX, 0, 0 }, "ParticleCircle", RRect(0, 64, 0, 64), { 0.5f, 0.5f }, 0xffe32e, 0, 0, speedB, speedB / 2, {0, 0, 0}, {1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, 0.1f, 0.1f, 0.2f, false);
					}
					ParticleSprite3D::Spawn({ posX, 0, 0 }, TextureManager::Load("./Resources/ptHexagon.png", "ParticleHexagon"), RRect(0, 512, 0, 512), { 0.5f, 0.5f }, 0xffe32e, Util::GetRand(0.0f, 360.0f), 180, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0.0f, 0.0f }, { 4.0f, 4.0f }, { 4.0f, 4.0f }, 0.1f, 0.1f, 0.1f, false);
					countJudgePerfect++;
					combo++;
					removeNotes.push_back(note);
					continue;
				}
				else if (!note.judgeFlag) {
					float diffA = time - music.ConvertBeatToMiliSeconds(note.mainBeat);
					float diffB = abs(diffA);

					if (existNearby[note.lane] && nearbyNotes[note.lane] == note) {
						if (RInput::GetKeyDown(laneKey[note.lane])
							|| (note.lane == 2 && RInput::GetMouseClickDown(0))
							|| (note.lane == 3 && RInput::GetMouseClickDown(1))
							|| (autoplay && diffA >= -judgeUltimatePerfect)) {
							if (diffB <= judgeHit * 2) {
								note.judgeFlag = true;
								note.judgeDiff = diffB;

								if (diffB <= judgePerfect) {
									//何もしない...んだけどこの書き方あほすぎんか
								}
								else if (diffB <= judgeHit) {
									RAudio::Play("JudgeHit");
									ParticleSprite::Spawn({ posX, 0, 0 }, "judgeText", RRect(9, 103, 69, 114), { 0.5f, 0.5f }, { 1, 1, 1, 1 }, 90, 50, 0, 0.5f, 0.5f, 0.2f);
									//TODO:ShowJudgeText(posX + laneWidth / 2, posJudgeLine + 20, StringFormat("%+.1f", diffA) + "ms", diffA > 0 ? 0xff0000 : 0x00ffff);
									for (int32_t i = 0; i < 12; i++) {
										ParticleExplode::Spawn({ posX, 0, 0 }, 0x77ff5e, (360.0f / 12) + (360.0f / 12 * i), 5, 15, 0.5f);
									}
									if (diffA > 0) {
										for (int32_t i = 0; i < 3; i++) {
											ParticleExplode::Spawn({ posX, 0, 0 }, 0xff0000, (360.0f / 3) + (360.0f / 3 * i), 5, 15, 0.5f);
										}
									}
									else {
										for (int32_t i = 0; i < 3; i++) {
											ParticleExplode::Spawn({ posX, 0, 0 }, 0x0000ff, (360.0f / 3) + (360.0f / 3 * i), 5, 15, 0.5f);
										}
									}
									countJudgeHit++;
									combo++;
									removeNotes.push_back(note);
									continue;
								}
								else {
									RAudio::Play("JudgeMiss");
									ParticleSprite::Spawn({ posX, 0, 0 }, "judgeText", RRect(10, 128, 128, 176), { 0.5f, 0.5f }, { 1, 1, 1, 1 }, 90, 50, 0, 0.5f, 0.5f, 0.2f);
									countJudgeMiss++;
									combo = 0;
									removeNotes.push_back(note);
									continue;
								}
							}
						}
					}

					if (diffA >= judgeHit * 2) {
						RAudio::Play("JudgeMiss");
						ParticleSprite::Spawn({ posX, 0, 0 }, "judgeText", RRect(10, 128, 128, 176), { 0.5f, 0.5f }, { 1, 1, 1, 1 }, 90, 50, 0, 0.5f, 0.5f, 0.2f);
						countJudgeMiss++;
						combo = 0;
						removeNotes.push_back(note);
						continue;
					}
				}
			}
			else if (note.type == Note::Types::Arc) {
				bool checkArcInputFlag = false;

				//全てのアークの節に対して
				Beat arcStartBeat, arcEndBeat;
				Vector3 arcStartPos, arcEndPos;
				arcStartBeat = note.mainBeat;
				arcStartPos = note.mainPos;
				for (auto& cp : note.controlPoints) {
					arcEndBeat = cp.beat;
					arcEndPos = cp.pos;
					if (CheckArcInput(arcStartBeat, arcEndBeat, arcStartPos, arcEndPos)) {
						checkArcInputFlag = true;
					}

					arcStartBeat = arcEndBeat;
					arcStartPos = arcEndPos;
				}
				arcEndBeat = note.subBeat;
				arcEndPos = note.subPos;
				if (CheckArcInput(arcStartBeat, arcEndBeat, arcStartPos, arcEndPos)) {
					checkArcInputFlag = true;
				}

				//アークに対して正しい入力をしているなら時間を記録してあげる
				if (checkArcInputFlag) {
					note.judgeFlag = true;
					note.HoldTime = time;
				}
				else {
					if (note.judgeFlag) {
						//猶予フレームを超えたなら終わり
						if (abs(time - note.HoldTime) > judgeHit * 3) {
							note.judgeFlag = false;
							note.HoldTime = time;
						}
					}
				}

				if (note.judgeFlag) {
					for (int32_t i = 0; i < 12; i++) {
						Vector3 speed = { Util::GetRand(-5.0f, 5.0f), Util::GetRand(-5.0f, 5.0f), 0 };
						Vector3 speedB = { 5.0f * cos(Util::AngleToRadian((360.0f / 12) + (360.0f / 12 * i))), 5.0f * sin(Util::AngleToRadian((360.0f / 4) + (360.0f / 4 * i))), 0 };
						ParticleSprite3D::Spawn(GetNowArcPos(note), "ParticleCircle", RRect(0, 64, 0, 64), { 0.5f, 0.5f }, { Util::GetRand(0.1f, 1.0f), Util::GetRand(0.1f, 1.0f), Util::GetRand(0.1f, 1.0f), 1 }, 0, 0, speedB * 5, speedB, { 0, 0, 0 }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f }, 0.1f, 0.1f, 0.2f, false);
						ParticleSprite3D::Spawn(GetNowArcPos(note), "ParticleCircle", RRect(0, 64, 0, 64), { 0.5f, 0.5f }, 0xffe32e, 0, 0, speedB, speedB / 2, { 0, 0, 0 }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f }, 0.1f, 0.1f, 0.2f, false);
					}
					ParticleSprite3D::Spawn(GetNowArcPos(note), TextureManager::Load("./Resources/ptHexagon.png", "ParticleHexagon"), RRect(0, 512, 0, 512), { 0.5f, 0.5f }, 0xffe32e, Util::GetRand(0.0f, 360.0f), 180, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0.0f, 0.0f }, { 4.0f, 4.0f }, { 4.0f, 4.0f }, 0.1f, 0.1f, 0.1f, false);

					/*for (int32_t i = 0; i < 4; i++) {
						ParticleExplode::Spawn(GetNowArcPos(note), 0xffe32e, (360.0f / 4) + (360.0f / 4 * i) + 45.0f, 5, 15, 0.5f);
					}*/
				}
				
				//一定間隔ごとに触れているか見てPerfectかMissに加算する
				float diffArcStart = time - music.ConvertBeatToMiliSeconds(note.mainBeat);
				if (diffArcStart >= 0) {
					float arcPoint = music.ConvertBeatToMiliSeconds(note.mainBeat);

					while (arcPoint <= music.ConvertBeatToMiliSeconds(note.subBeat)) {
						if (arcPoint <= note.judgeTime) {
							arcPoint += music.ConvertBeatToMiliSeconds({ 0, 1, 8 });
							continue;
						}

						float diff = time - arcPoint;

						if (diff >= 0) {
							if (note.judgeFlag) {
								countJudgePerfect++;
								combo++;
								note.judgeTime = arcPoint;
								ParticleSprite::Spawn(GetNowArcPos(note), "judgeText", RRect(9, 250, 10, 55), { 0.5f, 0.5f }, { 1, 1, 1, 1 }, 90, 30, 0, 0.5f, 0.5f, 0.2f);
								for (int32_t i = 0; i < 12; i++) {
									ParticleExplode::Spawn(GetNowArcPos(note), 0xffe32e, (360.0f / 12) + (360.0f / 12 * i), 5, 15, 0.5f);
								}
							}
							else {
								countJudgeMiss++;
								combo = 0;
								note.judgeTime = arcPoint;
								ParticleSprite::Spawn(GetNowArcPos(note), "judgeText", RRect(10, 128, 128, 176), { 0.5f, 0.5f }, { 1, 1, 1, 1 }, 90, 30, 0, 0.5f, 0.5f, 0.2f);
								for (int32_t i = 0; i < 12; i++) {
									ParticleExplode::Spawn(GetNowArcPos(note), 0xff0000, (360.0f / 12) + (360.0f / 12 * i), 5, 15, 0.5f);
								}
							}
						}
						else {
							break;
						}

						arcPoint += music.ConvertBeatToMiliSeconds({ 0, 1, 8 });
					}
				}

				if (note.judgeFlag) {
					note.judgeTime = time;
				}

				//直角の処理
				JudgeRightAngleArc(note);

				//TODO:続けて制御点毎の処理
				
				//終わってたら終わり
				if (music.ConvertBeatToMiliSeconds(note.subBeat) < time) {
					removeNotes.push_back(note);
				}
			}
		}

		//描画！？
		if (note.type == Note::Types::Tap) {
			if (posZ < -80 || posZ > 50) {
				continue;
			}

			if (noteObjIndex == noteModelObjs.size()) {
				noteModelObjs.emplace_back("Note");
			}
			ModelObj& obj = noteModelObjs[noteObjIndex];
			obj.mTransform.position = { posX, 0, -posZ };
			obj.mTransform.scale = { 0.85f, 1.0f, 1.15f };
			obj.mTransform.UpdateMatrix();
			obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
			obj.Draw();
			noteObjIndex++;
		}
		
		if (note.type == Note::Types::Arc) {
			Vector3 prevPos = note.mainPos;

			float startTime = 0;
			float endTime = music.ConvertBeatToMiliSeconds(note.mainBeat);
			Vector3 startPos;
			Vector3 endPos;

			Beat checkBeat;
			auto itr = note.controlPoints.begin();

			while (true) {
				startPos = prevPos;
				startTime = endTime;
				if (itr != note.controlPoints.end()) {
					endPos = itr->pos;
					checkBeat = itr->beat;
				}
				else {
					endPos = note.subPos;
					checkBeat = note.subBeat;
				}
				endTime = music.ConvertBeatToMiliSeconds(checkBeat);

				startPos.z = posJudgeLine + (startPos.z - nowPosY);
				endPos.z = posJudgeLine + (endPos.z - nowPosY);

				if (endTime <= note.judgeTime) {
					if (itr != note.controlPoints.end()) {
						prevPos = itr->pos;
						itr++;
						continue;
					}
					else {
						break;
					}
				}

				if (startPos.z > 80 || endPos.z < -50) {
					if (itr != note.controlPoints.end()) {
						prevPos = itr->pos;
						itr++;
						continue;
					}
					else {
						break;
					}
				}

				//判定済みの所までは常に進める
				if (startTime <= note.HoldTime && note.HoldTime <= endTime) {
					float ratio = Util::GetRatio(startTime, endTime, note.HoldTime);
					startPos = startPos + (endPos - startPos) * ratio;
				}

				if (note.judgeFlag && startPos.z <= posJudgeLine && endPos.z >= posJudgeLine) {
					float ratio = Util::GetRatio(startPos.z, endPos.z, posJudgeLine);
					startPos = startPos + (endPos - startPos) * ratio;
				}
				else if (startPos.z < -50) {
					float ratio = Util::GetRatio(startPos.z, endPos.z, -50);
					startPos = startPos + (endPos - startPos) * ratio;
				}

				if (endPos.z > 80) {
					float ratio = Util::GetRatio(startPos.z, endPos.z, 80);
					endPos = startPos + (endPos - startPos) * ratio;
				}

				if (arcObjIndex == arcNoteObjs.size()) {
					arcNoteObjs.emplace_back();
				}
				ArcNoteObj& obj = arcNoteObjs[arcObjIndex];
				obj.mStartPos = startPos;
				obj.mGoalPos = endPos;
				if (music.ConvertBeatToMiliSeconds(note.mainBeat) <= time
					&& !note.judgeFlag) {
					obj.mMaterial.mColor = { 1, 0, 1, 0.5f };
				}
				else {
					obj.mMaterial.mColor = { 1, 0, 1, 0.8f };
				}
				obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
				obj.Draw();
				arcObjIndex++;
				if (itr != note.controlPoints.end()) {
					prevPos = itr->pos;
					itr++;
				}
				else {
					break;
				}
			}
		}
	}

	for (Note& note : removeNotes) {
		remainNotes.remove(note);
	}

	DrawMeasureLine();

	//スコア計算
	if (countJudgePerfect == chart.noteCount) {
		score = 10000000;
	}
	else {
		int32_t monoScore = 10000000 / chart.noteCount;
		score = monoScore * countJudgePerfect + (monoScore / 2) * countJudgeHit;
	}

	maxCombo = max(combo, maxCombo);

	SimpleDrawer::DrawString(1280, 0, 5, "Score", { 1, 1, 1, 1 }, "", 25, { 1, 0 });
	SimpleDrawer::DrawString(1024, 20, 5, Util::StringFormat("%08d", score), { 1, 1, 1, 1 }, "", 60);
	SimpleDrawer::DrawString(20, 20, 5, Util::StringFormat("BPM:%.2f", music.GetBPM(beat)), {1, 1, 1, 1}, "", 60);
	SimpleDrawer::DrawString(20, 80, 5, Util::StringFormat("Perfect:%d", countJudgePerfect), {1, 1, 1, 1}, "", 25);
	SimpleDrawer::DrawString(20, 100, 5, Util::StringFormat("Hit:%d", countJudgeHit), { 1, 1, 1, 1 }, "", 25);
	SimpleDrawer::DrawString(20, 120, 5, Util::StringFormat("Hoge:%d", countJudgeMiss), { 1, 1, 1, 1 }, "", 25);

	if (combo != 0) {
		SimpleDrawer::DrawString(1070, 240, 0, "Combo", {1, 0.3f, 0, 1}, "", 25);
		SimpleDrawer::DrawString(1070, 260, 0, Util::StringFormat("%d", combo), { 1, 0.3f, 0, 1 }, "", 80);
	}
	SimpleDrawer::DrawString(1260, 80, 5, Util::StringFormat("MaxCombo:%d", maxCombo), { 1, 1, 1, 1 }, "", 25, {1, 0});
}

void RhythmGameController::Load()
{
	if (!chart.audiopath.empty()) {
		audioHandle = RAudio::Load(Util::ConvertWStringToString(PathUtil::ConvertAbsolute(Util::ConvertStringToWString(chart.audiopath), PathUtil::ConvertAbsolute(chart.path)).c_str()));
	}

	offsetStream = chart.audioOffset;

	music = chart.music;
	scrollChange = chart.scrollChange;
	cacheScrollPos.clear();

	notes = chart.notes;
	events = chart.events;
}

void RhythmGameController::Save()
{
	chart.audioOffset = offsetStream;

	chart.music = music;
	chart.scrollChange = scrollChange;

	chart.notes = notes;
	chart.Save();
}

bool RhythmGameController::CheckArcInput(Beat& arcStartBeat, Beat& arcEndBeat, Vector3& arcStartPos, Vector3& arcEndPos)
{
	//判定
	float diffA = time - music.ConvertBeatToMiliSeconds(arcStartBeat);
	float diffB = abs(diffA);

	Vector3 diffArcDir = (arcEndPos - arcStartPos);
	bool checkMouseDir = (diffArcDir.x == 0
		|| (RInput::GetMouseMove().x != 0 && RInput::GetMouseMove().x * diffArcDir.x >= 0))
		&& (diffArcDir.y == 0
			|| (RInput::GetMouseMove().y != 0 && RInput::GetMouseMove().y * diffArcDir.y < 0));

	if (checkMouseDir || autoplay) {
		if (diffA < 0) {
			if (diffB <= judgeHit) {
				return true;
			}
		}
		else {
			diffA = time - music.ConvertBeatToMiliSeconds(arcEndBeat);
			diffB = abs(diffA);

			if (diffA >= 0) {
				if (diffB <= judgeHit) {
					return true;
				}
			}
			else {
				return true;
			}
		}
	}

	return false;
}

void RhythmGameController::JudgeRightAngleArc(Note& note)
{
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

		if (arcStartTime <= note.judgeTimeB) {
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

		Vector3 diffPos = arcEndPos - arcStartPos;
		if (arcStartBeat == arcEndBeat) {
			float diffA = time - arcStartTime;
			float diffB = abs(diffA);

			if (arcStartTime <= time) {
				if (CheckArcInput(arcStartBeat, arcEndBeat, arcStartPos, arcEndPos)
					|| (note.judgeFlagB && note.judgeDiff <= judgeHit)) {
					countJudgePerfect++;
					combo++;
					note.judgeFlagB = false;
					note.judgeTimeB = arcStartTime;
					ParticleSprite::Spawn({ (arcStartPos.x + arcEndPos.x) / 2.0f, (arcStartPos.y + arcEndPos.y) / 2.0f, 0 }, "judgeText", RRect(9, 250, 10, 55), { 0.5f, 0.5f }, { 1, 1, 1, 1 }, 90, 20, 0, 0.5f, 0.5f, 0.2f); RAudio::Play("JudgePerfect");
					RAudio::Play("JudgePerfect");
					RAudio::Play("VerticalSlash");
					for (float t = 0; t < 1; t += 0.05f) {
						float fx = 1 - powf(1 - t, 3);

						Vector3 pos = arcStartPos * (1 - fx) + arcEndPos * fx;
						float speed = 30 * (1 - fx) + 10 * fx;
						pos.z = 0;
						float ang = Util::RadianToAngle(atan2f(diffPos.y, diffPos.x));
						ParticleExplode::Spawn(pos, 0xffe32e, ang + 180 + 30, speed, 30, 0.25f);
						ParticleExplode::Spawn(pos, 0xffe32e, ang + 180 - 30, speed, 30, 0.25f);
					}
				}
				else if (diffB > judgeHit) {
					countJudgeMiss++;
					combo = 0;
					note.judgeFlagB = false;
					note.judgeTimeB = arcStartTime;
					ParticleSprite::Spawn({ (arcStartPos.x + arcEndPos.x) / 2.0f, (arcStartPos.y + arcEndPos.y) / 2.0f, 0 }, "judgeText", RRect(10, 128, 128, 176), { 0.5f, 0.5f }, { 1, 1, 1, 1 }, 90, 20, 0, 0.5f, 0.5f, 0.2f);
					RAudio::Play("JudgeMiss");
				}
			}
			else {
				//先行入力
				if (CheckArcInput(arcStartBeat, arcEndBeat, arcStartPos, arcEndPos)) {
					note.judgeFlagB = true;
					note.judgeDiff = diffB;
				}
			}
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
}

Vector3 RhythmGameController::GetNowArcPos(Note& note)
{
	if (time < music.ConvertBeatToMiliSeconds(note.mainBeat)) {
		return note.mainPos;
	}
	//現在のアークの節を取得する
	Beat arcStartBeat = note.mainBeat;
	Beat arcEndBeat = note.subBeat;
	Vector3 arcStartPos, arcEndPos;
	arcStartPos = note.mainPos;
	arcEndPos = note.subPos;
	auto itr = note.controlPoints.begin();
	for (itr = note.controlPoints.begin(); itr != note.controlPoints.end();) {
		if (music.ConvertBeatToMiliSeconds(arcStartBeat) <= music.ConvertBeatToMiliSeconds(itr->beat)
			&& music.ConvertBeatToMiliSeconds(itr->beat) < time) {
			arcStartBeat = itr->beat;
			arcStartPos = itr->pos;
			itr++;
		}
		else {
			break;
		}
	}
	if (itr != note.controlPoints.end()) {
		arcEndBeat = itr->beat;
		arcEndPos = itr->pos;
	}
	else {
		arcEndBeat = note.subBeat;
		arcEndPos = note.subPos;
	}

	arcStartPos.z = 0;
	arcEndPos.z = 0;

	float ratio = Util::GetRatio(
		music.ConvertBeatToMiliSeconds(arcStartBeat),
		music.ConvertBeatToMiliSeconds(arcEndBeat),
		time);

	return arcStartPos + (arcEndPos - arcStartPos) * ratio;
}
