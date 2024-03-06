#pragma once
#include "Wax.h"

// 蝋群管理 //
class WaxManager final
{
private:
	bool isWaxDead = false;

public:
	std::vector<std::unique_ptr<Wax>> waxs;	//蝋ども
	const uint32_t kMaxWax = 128;	//最大弾数

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
		Vector3 originPos,float dist, Vector2 range, float size,float time);

	//最初の要素削除
	void EraceBegin();

	//満杯かどうか返す
	bool GetIsEmpty() { return waxs.size() >= kMaxWax; }

private:
	//コンストラクタ
	WaxManager() = default;
	//コピー禁止
	WaxManager(const WaxManager&) = delete;
	//デストラクタ
	~WaxManager() = default;
	//コピー禁止
	WaxManager& operator=(const WaxManager&) = delete;
};