#pragma once
#include "Wax.h"
#include "WaxGroup.h"

// 蝋群管理 //
class WaxManager final
{
private:
	bool isWaxDead = false;

	float heatBonus;			//上記の同時燃焼数に応じたボーナス係数
	//↑これは燃えた量が多ければ多いほど温度を保ちやすい、ってやつを
	//タカイが適当に解釈して適当にそれっぽくしてあります。
	//後でよりそれっぽいシステムにしてくださいプランナーさん

	std::string fileName = "Wax";

public:
	std::vector<std::unique_ptr<Wax>> waxs;	//蝋ども
	std::vector<std::unique_ptr<WaxGroup>> waxGroups;	//蝋ども
	const uint32_t kMaxWax = 128;	//最大グループ数

	int isBurningNum = 0;
	float heatUpTemperature;	//蝋が燃えたときに上がる温度

public:
	//シングルトンインスタンス取得
	static WaxManager* GetInstance();

	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw();

	//生成
	void Create(
		Transform transform, uint32_t power, Vector3 vec,
		float speed, Vector2 range, float size,float atkTime, float solidTime);

	//最初の要素削除
	void EraceBegin();

	void Move(uint32_t originNum,uint32_t moveNum);

	void Sort(uint32_t eraseNum);

	//現在の温度ボーナスを返す(計算済み)
	float GetCalcHeatBonus();

	//満杯かどうか返す
	bool GetIsFull() { return waxs.size() >= kMaxWax; }

private:
	//コンストラクタ
	WaxManager();
	//コピー禁止
	WaxManager(const WaxManager&) = delete;
	//デストラクタ
	~WaxManager() = default;
	//コピー禁止
	WaxManager& operator=(const WaxManager&) = delete;
};