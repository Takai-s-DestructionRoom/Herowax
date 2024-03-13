#include "WaxManager.h"
#include "ImGui.h"
#include "Temperature.h"
#include "Parameter.h"
#include "ColPrimitive3D.h"

WaxManager* WaxManager::GetInstance()
{
	static WaxManager instance;
	return &instance;
}

bool WaxManager::CheckHitWaxGroups(std::unique_ptr<WaxGroup>& group1,
	std::unique_ptr<WaxGroup>& group2)
{
	//一応内部で同じかどうか確認
	if (group1 == group2) {
		return false;
	}
	for (auto& wax1 : group1->waxs) {
		for (auto& wax2 : group2->waxs) {
			
			//どっちも固まり始めておらず
			if (!wax1->isSolid && !wax2->isSolid)
			{
				//どちらも地面にいないとダメ
				if (wax1->isGround && wax2->isGround)
				{
					//どれか一つでも当たっていたら
					if (ColPrimitive3D::CheckSphereToSphere(wax1->collider, wax2->collider))
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

WaxManager::WaxManager() :
	heatUpTemperature(10.f),
	heatBonus(5.f),
	waxDamage(1)
{
	//生成時に変数をセーブデータから引っ張ってくる
	std::map<std::string, std::string> extract = Parameter::Extract(fileName);
	heatUpTemperature = std::stof(extract["ロウが燃えたときの上昇温度"]);
	heatBonus = std::stof(extract["ボーナス上昇温度"]);
}

void WaxManager::Init()
{
	waxGroups.clear();
}

void WaxManager::Update()
{
	//燃えている数を初期化
	isBurningNum = 0;

	//死んでいるグループがあれば消す
	waxGroups.remove_if([](std::unique_ptr<WaxGroup>& wax) {
		return !wax->GetIsAlive();
		});

	for (auto& waxGroup : waxGroups)
	{
		waxGroup->Update();
	}

#pragma region ImGui
	ImGui::SetNextWindowSize({ 350, 180 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Wax", NULL, window_flags);
	ImGui::Text("存在しているロウの数:%d", (int)GetWaxNum());
	ImGui::Text("燃えているロウの数:%d", isBurningNum);
	ImGui::Text("現在の温度:%f", TemperatureManager::GetInstance()->GetTemperature());
	ImGui::PushItemWidth(100);
	ImGui::InputFloat("ロウが燃えたときの上昇温度", &heatUpTemperature, 1.0f);
	ImGui::InputFloat("ボーナス上昇温度", &heatBonus, 1.0f);

	ImGui::Text("ロウグループ数:%d", (int)waxGroups.size());
	for (auto& group : waxGroups)
	{
		ImGui::Text("グループ内のロウの数:%d", (int)group->waxs.size());
		ImGui::Text("グループの中で最長の固まる時間:%f", group->smallestTime);
		
	}
	ImGui::PopItemWidth();

	if (ImGui::Button("当たり判定の描画")) {
		isViewCol = !isViewCol;
	}

	if (ImGui::Button("Reset")) {
		Init();
	}
	if (ImGui::Button("セーブ")) {
		Parameter::Begin(fileName);
		Parameter::Save("ロウが燃えたときの上昇温度", heatUpTemperature);
		Parameter::Save("ボーナス上昇温度", heatBonus);
		Parameter::End();
	}

	ImGui::End();
#pragma endregion
}

void WaxManager::Draw()
{
	for (auto& group : waxGroups)
	{
		group->Draw();
		for (auto& wax : group->waxs)
		{
			wax->Draw();
			if (isViewCol) {
				wax->DrawCollider();
			}
		}
	}
}

void WaxManager::Create(Transform transform, uint32_t power, Vector3 vec,
	float speed, Vector2 range, float size, float atkTime, float solidTime)
{
	//生成時にグループ作成
	waxGroups.emplace_back();
	waxGroups.back() = std::make_unique<WaxGroup>();
	//生成したロウをグループへ格納
	waxGroups.back()->waxs.emplace_back();
	waxGroups.back()->waxs.back() = std::make_unique<Wax>();

	//指定された状態に
	waxGroups.back()->waxs.back()->obj.mTransform = transform;
	//情報を受け取って格納
	waxGroups.back()->waxs.back()->Init(power, vec, speed, range, size, atkTime, solidTime);
}

float WaxManager::GetCalcHeatBonus()
{
	return heatBonus * (float)isBurningNum;
}

uint32_t WaxManager::GetWaxNum()
{
	int32_t waxNum = 0;
	for (auto& group : waxGroups)
	{
		waxNum += (uint32_t)group->waxs.size();
	}
	return waxNum;
}
