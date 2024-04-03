#pragma once
#include "Wax.h"
#include "WaxGroup.h"
#include "ColPrimitive3D.h"

// 蝋群管理 //
class WaxManager final
{
private:
	bool isWaxDead = false;

	bool isViewCol = false;

	float heatBonus;			//上記の同時燃焼数に応じたボーナス係数
	//↑これは燃えた量が多ければ多いほど温度を保ちやすい、ってやつを
	//タカイが適当に解釈して適当にそれっぽくしてあります。
	//後でよりそれっぽいシステムにしてくださいプランナーさん

	std::string fileName = "Wax";

public:
	std::vector<float> waxTime;
	//std::vector<std::unique_ptr<Wax>> waxs;	//蝋ども
	std::list<std::unique_ptr<WaxGroup>> waxGroups;	//蝋ども
	const uint32_t kMaxWax = 128;	//最大グループ数

	uint32_t isBurningNum = 0;
	float heatUpTemperature;	//蝋が燃えたときに上がる温度
	uint32_t waxDamage;

	bool isCollected = true;	//回収し終えたか

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

	//現在の温度ボーナスを返す(計算済み)
	float GetCalcHeatBonus();

	//回収される
	bool Collect(ColPrimitive3D::Ray collider);
	
	uint32_t GetWaxNum();

	//満杯かどうか返す
	bool GetIsFull() { return GetWaxNum() >= kMaxWax; }

	bool CheckHitWaxGroups(std::unique_ptr<WaxGroup>& group1,
		std::unique_ptr<WaxGroup>& group2);

	GraphicsPipeline CreateDisolvePipeLine();

	//死んでるやつらを殺したりする
	void Delete();

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