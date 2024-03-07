#pragma once
#include "Vector2.h"

//温度描画UI
class TemperatureUI
{
public:
	Vector2 position;
	Vector2 size;

	float angle = -60.f;

public:
	TemperatureUI();

	static void LoadResource();	//リソース読み込み

	void Update();
	void Draw();
};

//温度を管理するクラス
class TemperatureManager
{
public:
	//シングルトンインスタンス取得
	static TemperatureManager* GetInstance();

	float GetTemperature() {return temperature;};
	void TemperaturePlus(float value) { temperature += value; };

	void Update();
	void Draw();

private:
	TemperatureManager();
	~TemperatureManager() {};

public:
	const float MAX_TEMPERATURE = 240.f;
private:
	TemperatureUI ui;		//描画UI
	float temperature;		//温度
	float downSpeed;		//1秒あたりに下がる温度

};

