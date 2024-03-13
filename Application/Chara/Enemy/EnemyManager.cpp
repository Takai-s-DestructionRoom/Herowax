#include "EnemyManager.h"
#include "RImGui.h"
#include "Colliders.h"
#include "Parameter.h"

EnemyManager* EnemyManager::GetInstance()
{
	static EnemyManager instance;
	return &instance;
}

EnemyManager::EnemyManager()
{
	std::map<std::string, std::string> extract = Parameter::Extract("Enemy");
	slowMag = Parameter::GetParam(extract,"減速率", 0.75f);
	slowCoatingMag = Parameter::GetParam(extract,"ろうまみれ減速率", 0.9f);
	burningBonus = Parameter::GetParam(extract,"敵が燃えたときのボーナス上昇温度",2.f);
}

void EnemyManager::CreateEnemy(const Vector3 position)
{
	enemys.emplace_back(target);
	enemys.back().SetPos(position);
	enemys.back().Init();
}

void EnemyManager::SetTarget(ModelObj* target_)
{
	target = target_;
}

void EnemyManager::LoadResource()
{
	EnemyUI::LoadResource();
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
		enemy.SetSlowMag(slowMag);	//減速率まとめて変更
		enemy.SetSlowCoatingMag(slowCoatingMag);	//蝋かかかった時の減速率まとめて変更
		enemy.Update();
	}

	burningComboTimer.Update();
	//猶予時間過ぎたらリセット
	if (burningComboTimer.GetEnd())
	{
		burningCombo = 0;
	}

	solidCombo = 0;	//同フレームで固まった敵のみカウントするので逐一0に戻す
	
#pragma region ImGui
	ImGui::SetNextWindowSize({ 300, 150 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Enemy", NULL, window_flags);

	ImGui::Text("パッドのLボタンでタワーの位置に敵出現");
	ImGui::Text("EnemyNum:%d", enemys.size());
	ImGui::SliderFloat("減速率", &slowMag, 0.f, 1.f);
	ImGui::SliderFloat("ろうまみれ減速率", &slowCoatingMag, 0.f, 1.f);
	ImGui::PushItemWidth(100);
	ImGui::InputFloat("敵が燃えたときのボーナス上昇温度", &burningBonus, 1.f);
	ImGui::PopItemWidth();

	if (ImGui::Button("Reset")) {
		enemys.clear();
	}
	if (ImGui::Button("セーブ")) {
		Parameter::Begin("Enemy");
		Parameter::Save("減速率", slowMag);
		Parameter::Save("ろうまみれ減速率", slowCoatingMag);
		Parameter::Save("敵が燃えたときのボーナス上昇温度", burningBonus);
		Parameter::End();
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
