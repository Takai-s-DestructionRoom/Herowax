#include "Temperature.h"
#include "Texture.h"
#include "InstantDrawer.h"
#include "RImGui.h"
#include "TimeManager.h"
#include "Parameter.h"
#include "SceneManager.h"
#include "ResultScene.h"

TemperatureManager* TemperatureManager::GetInstance()
{
	static TemperatureManager instance;
	return &instance;
}

TemperatureManager::TemperatureManager() :
	temperature(0),downSpeed(10), boaderTemperature(60), clearTimer(60),
	coldBorder(50), hotBorder(80), burningBorder(100)
{
	//生成時に変数をセーブデータから引っ張ってくる
	std::map<std::string, std::string> extract = Parameter::Extract("Temperature");
	downSpeed = stof(extract["一定時間(1秒)ごとの低下量"]);
	boaderTemperature = stof(extract["ボーダーライン"]);
	clearTimer.maxTime_ = stof(extract["クリアにかかる時間"]);

	MIN_TEMPERATURE = stof(extract["最低温度"]);
	MAX_TEMPERATURE = stof(extract["最大温度"]);
	coldBorder = stof(extract["冷えてる状態のボーダー"]);
	hotBorder = stof(extract["温かい状態のボーダー"]);
}

void TemperatureManager::Init()
{
	clearTimer.Reset();
	temperature = MIN_TEMPERATURE;
}

void TemperatureManager::Update()
{
	temperature -= downSpeed * TimeManager::deltaTime;
	temperature = Util::Clamp(temperature, MIN_TEMPERATURE, MAX_TEMPERATURE);

	burningBorder = TemperatureManager::GetInstance()->MAX_TEMPERATURE;

	ui.Update();

	//ボーダーラインを超えていたら時間が減る
	if (boaderTemperature <= temperature) {
		clearTimer.ReStart();
	}
	//超えてないなら止まる
	else{
		clearTimer.Stop();
	}
	if (clearTimer.GetEnd())
	{
		//クリアのフラグとか立てる
		SceneManager::GetInstance()->Change<ResultScene>();
	}
	clearTimer.Update();

#pragma region ImGui
	ImGui::SetNextWindowSize({ 350, 210 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("TemperatureUI", NULL, window_flags);

	ImGui::SliderFloat("温度", &temperature, MIN_TEMPERATURE, MAX_TEMPERATURE);
	ImGui::Text("残りクリア時間:%f", clearTimer.maxTime_ - clearTimer.nowTime_);
	ImGui::PushItemWidth(120);	//入力の枠小さくするやつ
	ImGui::InputFloat("一定時間(1秒)ごとの低下量", &downSpeed,1.f);
	ImGui::InputFloat("ボーダーライン", &boaderTemperature,1.f);
	ImGui::InputFloat("クリアにかかる時間", &clearTimer.maxTime_,1.f);
	ImGui::PopItemWidth();		//入力の枠元に戻すやつ
	if (ImGui::TreeNode("温度:詳細設定"))
	{
		ImGui::InputFloat("最低温度", &MIN_TEMPERATURE,1.f);
		ImGui::InputFloat("最大温度", &MAX_TEMPERATURE,1.f);

		ImGui::SliderFloat("冷えてる状態のボーダー", &coldBorder,
			TemperatureManager::GetInstance()->MIN_TEMPERATURE,
			TemperatureManager::GetInstance()->MAX_TEMPERATURE);
		ImGui::SliderFloat("温かい状態のボーダー", &hotBorder,
			TemperatureManager::GetInstance()->MIN_TEMPERATURE,
			TemperatureManager::GetInstance()->MAX_TEMPERATURE);
		ImGui::TreePop();
	}

	if (ImGui::Button("セーブ")) {
		Save();
	}

	ImGui::End();
#pragma endregion
}

void TemperatureManager::Save()
{
	Parameter::Begin("Temperature");
	Parameter::Save("一定時間(1秒)ごとの低下量", downSpeed);
	Parameter::Save("ボーダーライン", boaderTemperature);
	Parameter::Save("クリアにかかる時間", clearTimer.maxTime_);

	Parameter::Save("最低温度", MIN_TEMPERATURE);
	Parameter::Save("最大温度", MAX_TEMPERATURE);
	Parameter::Save("冷えてる状態のボーダー", coldBorder);
	Parameter::Save("温かい状態のボーダー", hotBorder);
	Parameter::End();
}


void TemperatureManager::Draw()
{
	ui.Draw();
}
