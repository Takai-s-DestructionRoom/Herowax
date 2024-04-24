#pragma once
#include "ModelObj.h"
#include "Enemy.h"
#include <list>
#include "Easing.h"
#include <memory>

class EnemyManager
{
public:
	//敵リスト
	std::list<std::unique_ptr<Enemy>> enemys;

	float knockRandXS;
	float knockRandXE;

	float knockRandZS;
	float knockRandZE;

private:
	uint32_t solidCombo;	//連続で固まった敵の数カウント(これが多いと抜け出すまでの時間減る)
	uint32_t burningCombo;	//連続で燃えた敵の数カウント(これが多いと温度爆上げ↑)

	Easing::EaseTimer burningComboTimer;	//燃焼コンボの繋がる猶予
	float burningBonus;						//燃焼コンボの上昇量

	float slowMag;			//共通の減速率
	float slowCoatingMag;	//共通の蝋かけられたときの減速率

	//------------ ノックバック関連 ------------//
	float knockRange;
	float knockTime;

	//------------ 無敵時間 -------------//
	float mutekiTime;

	Vector3 enemySize = { 3,3,3 };
	
	//調整項目
	float collideSize = 3;
	float attackHitColliderSize = 3.0f;
	float attackMove = 10.0f;
	float moveSpeed = 0.0f;
	
	//通常の敵の攻撃力
	float normalAtkPower = 1;
	//接触時のダメージ
	float contactAtkPower = 1;
	
	//攻撃していない時に敵と当たった場合でもダメージを入れる
	bool isContactDamage = false;

public:
	static EnemyManager* GetInstance();

	//指定した座標に敵を生成
	template <typename TEnemy>
	void CreateEnemy(const Vector3& position,const Vector3& scale,const Vector3& rotation,
		const std::string& behaviorOrder)
	{
		enemys.emplace_back();
		enemys.back() = std::make_unique<TEnemy>(target);
		enemys.back()->SetPos(position);
		enemys.back()->SetScale(scale);
		enemys.back()->SetRota(rotation);
		enemys.back()->SetBehaviorOrder(behaviorOrder);
		enemys.back()->Init();
	}

	//敵の追跡対象を変更(プレイヤーを入れるのを想定)
	void SetTarget(ModelObj* target_);
	
	static void LoadResource();

	//死んでるやつらを殺す
	void Delete();

	void Init();
	void Update();
	void Draw();

	// ゲッター //
	//連続で固まった敵の数カウント取得
	uint32_t GetSolidCombo() { return solidCombo; }
	//連続で燃えた敵の数カウント取得
	uint32_t GetBurningCombo() { return burningCombo; }
	//燃えた敵に応じて上昇するボーナスを取得
	float GetBurningBonus() { return burningBonus; }
	//攻撃に当たった時のノックバックする距離を取得
	float GetKnockBack() { return knockRange; };
	//攻撃力取得(今後種類が増えるなら、引数にenum入れる形に変更)
	float GetNormalAttackPower() { return normalAtkPower; };

	float GetContactAttackPower() { return contactAtkPower; };
	//攻撃していない時に敵と当たった場合でもダメージを入れる
	bool GetIsContactDamage() {return isContactDamage;};

	// セッター //
	//連続で固まった敵の数カウント増やす
	void IncrementSolidCombo() { solidCombo++; }
	//連続で燃えた敵の数カウント増やす
	void IncrementBurningCombo() { burningCombo++; burningComboTimer.Start(); }

private:
	EnemyManager();
	~EnemyManager() {};

	//コピー禁止
	EnemyManager& operator=(const EnemyManager&) = delete;

	//敵の追跡対象(プレイヤーを入れるのを想定)
	ModelObj* target = nullptr;

	Easing::EaseTimer spawnTimer;
};

