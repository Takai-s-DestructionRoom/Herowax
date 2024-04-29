#include "SpawnerManager.h"

SpawnerManager* SpawnerManager::GetInstance()
{
	static SpawnerManager instance;
	return &instance;
}

void SpawnerManager::Init()
{
	spawners.clear();
	spawnerPopManagetimer.maxTime_;
}

void SpawnerManager::Update()
{
	spawnerPopManagetimer.Update();

	//死んでるならリストから削除
	spawners.remove_if([](EnemySpawner& spawner) {
		return !spawner.GetIsAlive();
		});

	//スポナーを開始させる
	for (auto& spawner : spawners)
	{
		//すでに開始済みならスキップ
		if (spawner.GetStarted())continue;
		//スポナーの出現時間がタイミングを超えたら出現
		if (spawner.startTiming <= spawnerPopManagetimer.nowTime_) {
			spawner.Start();
		}
	}

	for (auto& spawner : spawners)
	{
		spawner.Update();
	}
}

void SpawnerManager::Draw()
{
	for (auto& spawner : spawners)
	{
		spawner.Draw();
	}
}

void SpawnerManager::Create(const Vector3& position, 
	const Vector3& scale, 
	const Vector3& rotation, 
	const std::string& loadfile,
	const std::string& behaviorOrder)
{
	spawners.emplace_back();
	spawners.back().Init(loadfile);
	spawners.back().SetPos(position);
	spawners.back().SetScale(scale);
	spawners.back().SetRota(rotation);
	spawners.back().LoadBehavior(behaviorOrder);
}

void SpawnerManager::Start(const std::vector<float>& startTimings)
{
	float biggerSpawn = 0;
	for (auto& spawner : SpawnerManager::GetInstance()->spawners)
	{
		if (biggerSpawn <= spawner.lifeTimer.maxTime_) {
			biggerSpawn = spawner.lifeTimer.maxTime_;
		}
	}

	float biggerTiming = 0;
	for (auto& timing : startTimings)
	{
		if (biggerTiming <= timing) {
			biggerTiming = timing;
		}
	}
	spawnerPopManagetimer.maxTime_ = biggerSpawn + biggerTiming;
	spawnerPopManagetimer.Start();
}
