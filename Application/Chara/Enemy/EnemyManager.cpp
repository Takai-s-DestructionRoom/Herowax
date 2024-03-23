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

	knockTime = Parameter::GetParam(extract, "ノックバックにかかる時間", 0.5f);
	knockRange = Parameter::GetParam(extract, "ノックバックする距離", 0.5f);

	knockRandXS = Parameter::GetParam(extract, "knockRandXS", -Util::PI / 2);
	knockRandXE = Parameter::GetParam(extract, "knockRandXE", Util::PI / 2);
	knockRandZS = Parameter::GetParam(extract, "knockRandZS", Util::PI / 4);
	knockRandZE = Parameter::GetParam(extract, "knockRandZE", Util::PI / 2);
	mutekiTime = Parameter::GetParam(extract, "無敵時間さん", 0.1f);
}

void EnemyManager::SetTarget(ModelObj* target_)
{
	target = target_;
}

void EnemyManager::SetGround(ModelObj* ground_)
{
	ground = ground_;
}

void EnemyManager::LoadResource()
{
	EnemyUI::LoadResource();
}

void EnemyManager::Init()
{
	enemys.clear();
}

void EnemyManager::Update()
{
	Delete();

	for (auto& enemy : enemys)
	{
		enemy->SetSlowMag(slowMag);	//減速率まとめて変更
		enemy->SetSlowCoatingMag(slowCoatingMag);	//蝋かかかった時の減速率まとめて変更
		enemy->SetKnockRange(knockRange);
		enemy->SetKnockTime(knockTime);
		enemy->SetMutekiTime(mutekiTime);
			 
		enemy->Update();
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
	ImGui::SliderFloat("ノックバックにかかる時間", &knockTime,0.0f,5.0f);
	ImGui::SliderFloat("ノックバックする距離", &knockRange,0.0f,10.f);

	ImGui::SliderFloat("knockRandXS", &knockRandXS, -Util::PI, Util::PI);
	ImGui::SliderFloat("knockRandXE", &knockRandXE, -Util::PI, Util::PI);
	ImGui::SliderFloat("knockRandZS", &knockRandZS, -Util::PI, Util::PI);
	ImGui::SliderFloat("knockRandZE", &knockRandZE, -Util::PI, Util::PI);
	ImGui::SliderFloat("knockRandZE", &knockRandZE, -Util::PI, Util::PI);
	ImGui::SliderFloat("無敵時間さん", &mutekiTime, 0.0f,1.0f);

	for (auto& enemy : enemys)
	{
		ImGui::Text("ステート:%s", enemy->GetState().c_str());
	}

	if (ImGui::Button("Reset")) {
		enemys.clear();
	}
	if (ImGui::Button("セーブ")) {
		Parameter::Begin("Enemy");
		Parameter::Save("減速率", slowMag);
		Parameter::Save("ろうまみれ減速率", slowCoatingMag);
		Parameter::Save("敵が燃えたときのボーナス上昇温度", burningBonus);
		Parameter::Save("ノックバックにかかる時間", knockTime);
		Parameter::Save("ノックバックする距離", knockRange);
		Parameter::Save("knockRandXS", knockRandXS);
		Parameter::Save("knockRandXE", knockRandXE);
		Parameter::Save("knockRandZS", knockRandZS);
		Parameter::Save("knockRandZE", knockRandZE);
		Parameter::Save("無敵時間さん", mutekiTime);

		Parameter::End();
	}

	ImGui::End();
#pragma endregion
}

void EnemyManager::Draw()
{
	for (auto& enemy : enemys)
	{
		enemy->Draw();
	}
}

void EnemyManager::Delete()
{
	//死んでるならリストから削除
	enemys.remove_if([](std::unique_ptr<Enemy>& enemy) {
		return !enemy->GetIsAlive();
		});
}
