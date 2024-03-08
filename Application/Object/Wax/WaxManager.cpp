#include "WaxManager.h"
#include "ImGui.h"
#include "Temperature.h"

WaxManager* WaxManager::GetInstance()
{
	static WaxManager instance;
	return &instance;
}

WaxManager::WaxManager() : 
	heatUpTemperature(10.f),
	heatBonus(5.f)
{

}

void WaxManager::Init()
{
	waxs.clear();
}

void WaxManager::Update()
{
	//寿命が尽きた蝋を全削除
	for (uint32_t i = 0; i < waxs.size(); i++)
	{
		if (waxs[i]->GetIsAlive() == false)
		{
			waxs.erase(waxs.begin() + i);
			i--;
		}
	}

	//燃えている数を初期化
	isBurningNum = 0;

	for (auto& wax : waxs)
	{
		wax->Update();
	}

#pragma region ImGui
	ImGui::SetNextWindowSize({ 350, 180 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Wax", NULL, window_flags);
	ImGui::Text("存在しているロウの数:%d", (int)waxs.size());
	ImGui::Text("燃えているロウの数:%d", isBurningNum);
	ImGui::Text("現在の温度:%f", TemperatureManager::GetInstance()->GetTemperature());
	ImGui::PushItemWidth(100);
	ImGui::InputFloat("ロウが燃えたときの上昇温度", &heatUpTemperature,1.0f);
	ImGui::InputFloat("ボーナス上昇温度", &heatBonus, 1.0f);
	ImGui::PopItemWidth();

	if (ImGui::Button("Reset")) {
		Init();
	}

	ImGui::End();
#pragma endregion
}

void WaxManager::Draw()
{
	for (auto& wax : waxs)
	{
		wax->Draw();
	}
}

void WaxManager::Create(Transform transform, uint32_t power, Vector3 vec,
	float speed, Vector2 range, float size, float time)
{
	//要素追加
	waxs.emplace_back();
	waxs.back() = std::make_unique<Wax>();

	//指定された状態に
	waxs.back()->obj.mTransform = transform;
	//情報を受け取って格納
	waxs.back()->Init(power, vec, speed, range, size, time);
}

void WaxManager::EraceBegin()
{
	waxs.erase(waxs.begin());
}

float WaxManager::GetCalcHeatBonus()
{
	return heatBonus * (float)isBurningNum;
}
