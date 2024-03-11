#pragma once
#include "Vector2.h"
#include "Easing.h"

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
	float MIN_TEMPERATURE = 40.f;
	float MAX_TEMPERATURE = 100.f;
private:
	TemperatureUI ui;		//描画UI
	float temperature;		//温度
	float downSpeed;		//1秒あたりに下がる温度
	float boaderTemperature;		//この温度以上を保つとクリア時間が減るボーダーライン
	Easing::EaseTimer clearTimer;	//減らしきったらクリアになる時間

	void Save();//一度もセーブしたことのない値を読み込もうとするとエラーが出るので、
				//関数にまとめてコンストラクタで一回呼び出してあげると、
				//変数で定義しておいた値を入れられるので安全
};

