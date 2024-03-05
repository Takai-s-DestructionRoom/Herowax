#include "EnemyManager.h"
#include "RImGui.h"
#include "Colliders.h"

EnemyManager* EnemyManager::GetInstance()
{
	static EnemyManager instance;
	return &instance;
}

void EnemyManager::CreateEnemy(const Vector3 position)
{
	enemys.emplace_back(target);
	enemys.back().SetPos(position);
}

void EnemyManager::SetTarget(ModelObj* target_)
{
	target = target_;
}

void EnemyManager::Init()
{

}

void EnemyManager::Update()
{
	//死んでるならリストから削除
	enemys.remove_if([](Enemy& enemy) {
		return !enemy.GetIsAlive();
		});
	
	for (auto& enemy : enemys)
	{
		enemy.Update();
	}
	
#pragma region ImGui
	ImGui::SetNextWindowSize({ 300, 100 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Enemy", NULL, window_flags);

	ImGui::Text("パッドのLボタンでタワーの位置に敵出現");
	ImGui::Text("EnemyNum:%d", enemys.size());

	if (ImGui::Button("Reset")) {
		enemys.clear();
	}

	ImGui::End();
#pragma endregion
}

void EnemyManager::Draw()
{
	for (auto& enemy : enemys)
	{
		enemy.Draw();
	}
}
