#pragma once
#include "Vector2.h"

//温度描画UI
class TemperatureUI
{
public:
	Vector2 position;
	Vector2 size;

public:
	TemperatureUI();

	static void LoadResource();	//リソース読み込み

	void Update();
	void Draw();

	void ImGui();
};

//温度を管理するクラス
class TemperatureManager
{
public:
	//シングルトンインスタンス取得
	static TemperatureManager* GetInstance();

	float GetTemperature() {return temperature;};
	void SetTemperature(float value) { temperature = value; };

	void UpdateUI();
	void DrawUI();

private:
	TemperatureUI ui;		//描画UI
	float temperature;		//温度
};

