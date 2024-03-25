#pragma once
#include "Vector2.h"
#include "Easing.h"
#include "TemperatueUI.h"

//温度を管理するクラス
class TemperatureManager
{
public:
	//シングルトンインスタンス取得
	static TemperatureManager* GetInstance();

	void Init();
	void Update();
	void Draw();

	void TemperaturePlus(float value);
	
	// ゲッター //
	//現在の温度取得
	float GetTemperature() {return temperature;};
	//冷えてる温度(青)のボーダー取得
	float GetColdBorder() { return coldBorder; };
	//暖かい温度(黄色)のボーダー取得
	float GetHotBorder() { return hotBorder; };
	//めっちゃ熱い温度(赤)のボーダー取得
	float GetBurningBorder() { return burningBorder; };

private:
	TemperatureManager();
	~TemperatureManager() {};

public:
	float START_TEMPERATURE = 0.0f;
	float MIN_TEMPERATURE = 40.f;
	float MAX_TEMPERATURE = 100.f;
private:
	TemperatureUI ui;		//描画UI
	float plusTemperature;
	float maxPlusTemp;
	float temperature;		//温度
	float downSpeed;		//1秒あたりに下がる温度
	float boaderTemperature;		//この温度以上を保つとクリア時間が減るボーダーライン
	Easing::EaseTimer clearTimer;	//減らしきったらクリアになる時間

	float coldBorder;			//冷えてる色のボーダー
	float hotBorder;			//暖かい色のボーダー
	float burningBorder;		//めっちゃ熱い色のボーダー

	void Save();//一度もセーブしたことのない値を読み込もうとするとエラーが出るので、
				//関数にまとめてコンストラクタで一回呼び出してあげると、
				//変数で定義しておいた値を入れられるので安全
};

