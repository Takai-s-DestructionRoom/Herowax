#pragma once
#include "Sprite.h"
#include "Easing.h"

// シーン遷移 //

class SceneTrance
{
private:
	std::unique_ptr<Sprite> blackBack_;	//背景の黒い奴

	bool isSceneTrance_;	//シーン切り替えるかフラグ
	bool isSceneTranceNow_;	//シーン遷移中かフラグ

	Easing::EaseTimer inTimer_ = 0.5f;		//入ってくるときのタイマー
	Easing::EaseTimer outTimer_ = 0.5f;	//出てくときのタイマー

	Vector2 pos_;	//暗幕の座標

public:
	//コンストラクタ
	SceneTrance();
	//シングルトンインスタンス取得
	static SceneTrance* GetInstance();

	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw();

	//シーン遷移開始！
	void Start();

	// ゲッター //
	//シーン切り替えるかフラグ取得
	bool GetIsChange()const { return isSceneTrance_; }
	//シーン遷移中かフラグ取得
	bool GetIsChangeNow()const { return isSceneTranceNow_; }

	// セッター //
	//シーン切り替えるかフラグ設定
	void SetIsChange(bool isChange) { this->isSceneTrance_ = isChange; }

private:
	//コピー禁止
	SceneTrance(const SceneTrance&) = delete;
	//デストラクタ
	~SceneTrance() = default;
	//コピー禁止
	SceneTrance& operator=(const SceneTrance&) = delete;
};