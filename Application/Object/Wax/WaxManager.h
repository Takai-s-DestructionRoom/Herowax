#pragma once
#include "Wax.h"
#include "WaxGroup.h"
#include "ColPrimitive3D.h"
#include "SlimeWax.h"

// 蝋群管理 //
class WaxManager final
{
private:
	bool isWaxDead = false;

	bool isViewCol = false;			//当たり判定の表示フラグ

	bool isViewSlimeWax = true;		//スライムロウ(レイマーチ描画)の表示フラグ

	bool isViewObjectWax = false;	//オブジェクトロウ(モデル描画)の表示フラグ

	float colliderSize = 0.0f;

	float heatBonus;			//上記の同時燃焼数に応じたボーナス係数
	//↑これは燃えた量が多ければ多いほど温度を保ちやすい、ってやつを
	//タカイが適当に解釈して適当にそれっぽくしてあります。
	//後でよりそれっぽいシステムにしてくださいプランナーさん

	std::string fileName = "Wax";

	//調整項目
	float slimeWaxSizeMag = 1.0f;	//ロウのサイズを全部統一で小さくする
	
public:
	std::vector<float> waxTime;
	//std::vector<std::unique_ptr<Wax>> waxs;	//蝋ども
	std::list<std::unique_ptr<WaxGroup>> waxGroups;	//蝋ども
	const uint32_t kMaxWax = 128;	//最大グループ数

	uint32_t isBurningNum = 0;
	float heatUpTemperature;	//蝋が燃えたときに上がる温度
	uint32_t waxDamage;

	bool notCollect = true;	//回収し終えたか
	float accelAmount = 0.1f;	//回収の加速度合い
	
	float collectTime = 1.0f;		//吸収されるまでの時間

	SlimeWax slimeWax;

	int32_t maxWaxStock = 0;
	int32_t collectWaxNum = 0;

	GameObject* collectTarget = nullptr;

public:
	//シングルトンインスタンス取得
	static WaxManager* GetInstance();

	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw();

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="transform">生成座標、大きさなど</param>
	/// <param name="vec">飛ばす方向</param>
	/// <param name="targetPos"></param>
	/// <param name="range"></param>
	/// <param name="size">大きさ</param>
	/// <param name="atkTime">攻撃が地面に落ちるまでの時間</param>
	/// <param name="solidTime">固まるまでの時間</param>
	void Create(Transform transform, Vector3 endPos, float height, float size, float atkTime, float solidTime);

	//現在の温度ボーナスを返す(計算済み)
	float GetCalcHeatBonus();

	//回収される
	bool Collect(ColPrimitive3D::Ray collider);
	/// <summary>
	/// 回収される(縦幅指定バージョン)
	/// </summary>
	/// <param name="collider"></param>
	/// <param name="waxCollectVertical"></param>
	/// <returns>回収した数</returns>
	int32_t Collect(ColPrimitive3D::Ray collider,float waxCollectVertical,GameObject* target);

	uint32_t GetWaxNum();

	//満杯かどうか返す
	bool GetIsFull() { return GetWaxNum() >= kMaxWax; }
	float GetSlimeWaxSizeMag() { return slimeWaxSizeMag; }

	bool CheckHitWaxGroups(std::unique_ptr<WaxGroup>& group1,
		std::unique_ptr<WaxGroup>& group2);

	GraphicsPipeline CreateDisolvePipeLine();

	GraphicsPipeline SlimeShaderPipeLine();	//パイプライン生成

	//死んでるやつらを殺したりする
	void Delete();
	//ロウの総量設定
	void SetMaxWaxStock(int32_t maxWax) { maxWaxStock = maxWax; }

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