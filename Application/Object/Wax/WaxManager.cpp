#include "WaxManager.h"
#include "ImGui.h"
#include "Temperature.h"
#include "Parameter.h"

WaxManager* WaxManager::GetInstance()
{
	static WaxManager instance;
	return &instance;
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
	waxs.clear();
	waxGroups.clear();
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

	for (uint32_t i = 0; i < waxGroups.size(); i++)
	{
		//ロウグループが空なら
		if (waxGroups[i]->GetIsEmpty())
		{
			//そのロウグループに所属してるロウ皆殺し
			for (uint32_t j = 0; j < waxs.size(); j++)
			{
				if (waxs[j]->groupNum == i)
				{
					waxs[j]->isAlive = false;
				}
			}
			//要素削除
			waxGroups.erase(waxGroups.begin() + i);
		}
	}

	//燃えている数を初期化
	isBurningNum = 0;

	for (auto& wax : waxs)
	{
		wax->Update();
	}

	for (auto& waxGroup : waxGroups)
	{
		waxGroup->Update();
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
	ImGui::InputFloat("ロウが燃えたときの上昇温度", &heatUpTemperature, 1.0f);
	ImGui::InputFloat("ボーナス上昇温度", &heatBonus, 1.0f);

	ImGui::Text("ロウグループ数:%d", (int)waxGroups.size());
	for (uint32_t i = 0; i < waxGroups.size(); i++)
	{
		ImGui::Text("グループ内のロウの数:%d", (int)waxGroups[i]->waxNums.size());
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
	for (auto& wax : waxs)
	{
		wax->Draw();
		if (isViewCol) {
			wax->DrawCollider();
		}
	}
}

void WaxManager::Create(Transform transform, uint32_t power, Vector3 vec,
	float speed, Vector2 range, float size, float atkTime, float solidTime)
{
	//要素追加
	waxs.emplace_back();
	waxs.back() = std::make_unique<Wax>();
	waxs.back()->groupNum = (uint32_t)waxGroups.size();

	//ロウグループも逐一追加
	waxGroups.emplace_back();
	waxGroups.back() = std::make_unique<WaxGroup>();
	//ロウグループが保持するロウの要素番号は今生成したばかりなのでsize-1
	waxGroups.back()->waxNums.emplace_back();
	waxGroups.back()->waxNums.back() = (uint32_t)waxs.size() - 1;


	//指定された状態に
	waxs.back()->obj.mTransform = transform;
	//情報を受け取って格納
	waxs.back()->Init(power, vec, speed, range, size, atkTime, solidTime);
}

void WaxManager::EraceBegin()
{
	waxs.erase(waxs.begin());
}

void WaxManager::Move(uint32_t originNum, uint32_t moveNum)
{
	if (waxGroups[originNum]->waxNums.empty() == false &&
		waxGroups[moveNum]->waxNums.empty() == false)
	{
		//要素の最後から移動させたい奴を全部詰める
		waxGroups[originNum]->waxNums.insert(
			waxGroups[originNum]->waxNums.end(),
			waxGroups[moveNum]->waxNums.begin(),
			waxGroups[moveNum]->waxNums.end());

		//移動が終わったら移動元は消す
		waxGroups[moveNum]->waxNums.clear();

		//くっついてるロウの数だけHP増やす
		waxGroups[originNum]->SetHP(waxGroups[originNum]->waxNums.size() * 10.f);

		//ロウに割り振られてるグループ番号を再設定
		for (auto& num : waxGroups[originNum]->waxNums)
		{
			waxs[num]->groupNum = originNum;
		}
	}
}

float WaxManager::GetCalcHeatBonus()
{
	return heatBonus * (float)isBurningNum;
}
