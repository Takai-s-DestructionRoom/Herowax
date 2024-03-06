#include "WaxManager.h"
#include "ImGui.h"

WaxManager* WaxManager::GetInstance()
{
	static WaxManager instance;
	return &instance;
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

	for (auto& wax : waxs)
	{
		wax->Update();
	}

#pragma region ImGui
	ImGui::SetNextWindowSize({ 100, 70 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Wax", NULL, window_flags);

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
	Vector3 originPos, float dist, Vector2 range, float size, float time)
{
	//要素追加
	waxs.emplace_back();
	waxs.back() = std::make_unique<Wax>();

	//指定された状態に
	waxs.back()->obj.mTransform = transform;
	//情報を受け取って格納
	waxs.back()->Init(power, vec, originPos, dist, range, size, time);
}

void WaxManager::EraceBegin()
{
	waxs.erase(waxs.begin());
}
