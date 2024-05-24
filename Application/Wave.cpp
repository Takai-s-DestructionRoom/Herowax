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
		"test2",
	};

	//ウェーブ読み込み
	LevelLoader::Get()->Load("./Level/wave/wave1.json", waves[0]);
	LevelLoader::Get()->Load("./Level/wave/wave2.json", waves[1]);
	LevelLoader::Get()->Load("./Level/wave/test2.json", waves[2]);
}

void WaveManager::Update()
{
	waitTimer.Update();

	if (waitTimer.GetNowEnd()) {
		
		//敵全員を動かし始める
		for (auto& enemy : EnemyManager::GetInstance()->enemys)
		{
			enemy->StartToMoving();
		}
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
	waitTimer.maxTime_ = Level::Get()->waveWaitTime;
	waitTimer.Start();
}