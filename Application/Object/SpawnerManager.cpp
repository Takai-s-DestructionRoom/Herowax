#include "SpawnerManager.h"

SpawnerManager* SpawnerManager::GetInstance()
{
	static SpawnerManager instance;
	return &instance;
}

void SpawnerManager::Init()
{
	spawners.clear();
}

void SpawnerManager::Update()
{
	//死んでるならリストから削除
	spawners.remove_if([](EnemySpawner& spawner) {
		return !spawner.GetIsAlive();
		});

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

void SpawnerManager::Create(const Vector3& position, const Vector3& scale, const Vector3& rotation, const std::string& loadfile)
{
	spawners.emplace_back();
	spawners.back().Init(loadfile);
	spawners.back().SetPos(position);
	spawners.back().SetScale(scale);
	spawners.back().SetRota(rotation);
}
