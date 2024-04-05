#include "Temperature.h"
#include "Texture.h"
#include "InstantDrawer.h"
#include "RImGui.h"
#include "TimeManager.h"
#include "Parameter.h"
#include "SceneManager.h"
#include "ResultScene.h"
#include "SimpleSceneTransition.h"

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
	downSpeed = Parameter::GetParam(extract, "1秒ごとの低下量",10.f);
	maxPlusTemp = Parameter::GetParam(extract, "1秒ごとに上がる最大値",10.f);
	boaderTemperature = Parameter::GetParam(extract, "クリアタイマーが減るボーダーライン",60.f);
	clearTimer.maxTime_ = Parameter::GetParam(extract, "クリアにかかる時間",60.f);

	START_TEMPERATURE = Parameter::GetParam(extract,"開始時の温度",40.f);
	MIN_TEMPERATURE = Parameter::GetParam(extract,"最低温度",40.f);
	MAX_TEMPERATURE = Parameter::GetParam(extract,"最大温度",100.f);
	coldBorder = Parameter::GetParam(extract, "冷えてる状態のボーダー",50.f);
	hotBorder = Parameter::GetParam(extract, "温かい状態のボーダー",80.f);
}

void TemperatureManager::Init()
{
	clearTimer.Reset();
	temperature = START_TEMPERATURE;
	temperature = Util::Clamp(temperature, MIN_TEMPERATURE, MAX_TEMPERATURE);
}

void TemperatureManager::Update()
{
	temperature += plusTemperature * TimeManager::deltaTime;

	plusTemperature -= maxPlusTemp * TimeManager::deltaTime;
	plusTemperature = Util::Clamp(plusTemperature, 0.f, maxPlusTemp);

	if (plusTemperature <= 0) {
		temperature -= downSpeed * TimeManager::deltaTime;
		temperature = Util::Clamp(temperature, MIN_TEMPERATURE, MAX_TEMPERATURE);
	}

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
		//デバッグ中なら遷移しない
		if (!Util::debugBool) {
			//クリアのフラグとか立てる
			SceneManager::GetInstance()->Change<ResultScene>();
		}
	}
	clearTimer.Update();

	//温度0になったらリザルトへ飛ばす
	if (temperature <= MIN_TEMPERATURE)
	{
		Init();

		//デバッグ中なら遷移しない
		if (!Util::debugBool) {
			SceneManager::GetInstance()->Change<ResultScene, SimpleSceneTransition>();
		}
	}

//#pragma region ImGui
//	ImGui::SetNextWindowSize({ 350, 210 });
//
//	ImGuiWindowFlags window_flags = 0;
//	window_flags |= ImGuiWindowFlags_NoResize;
//
//	ImGui::Begin("TemperatureUI", NULL, window_flags);
//
//	ImGui::SliderFloat("温度", &temperature, MIN_TEMPERATURE, MAX_TEMPERATURE);
//	ImGui::Text("残りクリア時間:%f", clearTimer.maxTime_ - clearTimer.nowTime_);
//	ImGui::PushItemWidth(120);	//入力の枠小さくするやつ
//	ImGui::InputFloat("1秒ごとの低下量", &downSpeed,1.f);
//	ImGui::InputFloat("1秒ごとに上がる最大値",&maxPlusTemp, 1.f);
//	ImGui::InputFloat("クリアタイマーが減るボーダーライン", &boaderTemperature,1.f);
//	ImGui::InputFloat("クリアにかかる時間", &clearTimer.maxTime_,1.f);
//	ImGui::InputFloat("開始時の温度", &START_TEMPERATURE,1.f);
//	ImGui::PopItemWidth();		//入力の枠元に戻すやつ
//	if (ImGui::TreeNode("温度:詳細設定"))
//	{
//		ImGui::InputFloat("最低温度", &MIN_TEMPERATURE,1.f);
//		ImGui::InputFloat("最大温度", &MAX_TEMPERATURE,1.f);
//
//		ImGui::SliderFloat("冷えてる状態のボーダー", &coldBorder,
//			TemperatureManager::GetInstance()->MIN_TEMPERATURE,
//			TemperatureManager::GetInstance()->MAX_TEMPERATURE);
//		ImGui::SliderFloat("温かい状態のボーダー", &hotBorder,
//			TemperatureManager::GetInstance()->MIN_TEMPERATURE,
//			TemperatureManager::GetInstance()->MAX_TEMPERATURE);
//		ImGui::TreePop();
//	}
//
//	if (ImGui::Button("セーブ")) {
//		Save();
//	}
//
//	ImGui::End();
//#pragma endregion
}

void TemperatureManager::Save()
{
	Parameter::Begin("Temperature");
	Parameter::Save("1秒ごとの低下量", downSpeed);
	Parameter::Save("1秒ごとに上がる最大値", maxPlusTemp);
	Parameter::Save("クリアタイマーが減るボーダーライン", boaderTemperature);
	Parameter::Save("クリアにかかる時間", clearTimer.maxTime_);

	Parameter::Save("開始時の温度", START_TEMPERATURE);
	Parameter::Save("最低温度", MIN_TEMPERATURE);
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

void TemperatureManager::TemperaturePlus(float value)
{
	plusTemperature += value;
	plusTemperature = Util::Clamp(plusTemperature, 0.f, maxPlusTemp);
}
