#include "Wave.h"
#include "SpawnerManager.h"
#include "LevelLoader.h"
#include "Level.h"
#include "RImGui.h"
#include "RInput.h"
#include "EnemyManager.h"

WaveManager* WaveManager::Get()
{
	static WaveManager instance;
	return &instance;
}

void WaveManager::LoadLevelData()
{
	waves = {
		"wave1",
		"wave2",
		"wave3",
		"wave4",
		"wave5",
		"wave6",
		"wave7",
		"wave8",
	};

	//ウェーブ読み込み
	LevelLoader::Get()->Load("./Level/wave/wave1.json", waves[0]);
	LevelLoader::Get()->Load("./Level/wave/wave2.json", waves[1]);
	LevelLoader::Get()->Load("./Level/wave/wave3.json", waves[2]);
	LevelLoader::Get()->Load("./Level/wave/wave4.json", waves[3]);
	LevelLoader::Get()->Load("./Level/wave/wave5.json", waves[4]);
	LevelLoader::Get()->Load("./Level/wave/wave6.json", waves[5]);
	LevelLoader::Get()->Load("./Level/wave/wave7.json", waves[6]);
	LevelLoader::Get()->Load("./Level/wave/wave8.json", waves[7]);
}

void WaveManager::Update()
{
	waitTimer.Update();

	if (waitTimer.GetNowEnd() || zeroFlag) {
		
		//敵全員を動かし始める
		for (auto& enemy : EnemyManager::GetInstance()->enemys)
		{
			enemy->StartToMoving();
		}

		zeroFlag = false;
	}

	if (EnemyManager::GetInstance()->enemys.size() <= 0)
	{
		NextWave();
	}

	Imgui();
}

void WaveManager::Imgui()
{
	if (RImGui::showImGui) {
		ImGui::Begin("WaveManager");
		ImGui::Text("敵の数:%d", (int32_t)EnemyManager::GetInstance()->enemys.size());
		ImGui::Text("waitTimer:%f", waitTimer.GetTimeRate());
		if (ImGui::Button("敵を全消し(次のウェーブに進める)")) {
			EnemyManager::GetInstance()->enemys.clear();
		}

		ImGui::SliderInt("ウェーブを指定する", &debugNum, 1, MAX_NUM);
		if (ImGui::Button("指定したウェーブへ移動")) {
			MoveWave(debugNum - 1);
		}

		ImGui::End();
	}
}

void WaveManager::NextWave()
{
	waveNum++;
	if (waveNum >= MAX_NUM) {
		waveNum = 0;
	}
	Level::Get()->Extract(waves[waveNum]);
	//0より大きいなら入れる
	if (Level::Get()->waveWaitTime > 0.0f) {
		waitTimer.maxTime_ = Level::Get()->waveWaitTime;
		waitTimer.Start();
	}
	//小さいなら別フラグを立てる
	else {
		zeroFlag = true;
	}
}

void WaveManager::MoveWave(int32_t moveNum)
{
	Level::Get()->Extract(waves[moveNum]);
	//0より大きいなら入れる
	
	Level::Get()->waveWaitTime = 0.0f;
	if (Level::Get()->waveWaitTime > 0.0f) {
		waitTimer.maxTime_ = Level::Get()->waveWaitTime;
		waitTimer.Start();
	}
	//小さいなら別フラグを立てる
	else {
		zeroFlag = true;
	}
}
