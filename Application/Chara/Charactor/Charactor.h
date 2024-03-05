#pragma once
#include "ModelObj.h"

//Player,Enemyなど生きてる系オブジェクトの基底クラス
class Charactor
{
public:
	//------------ 移動関連 ------------//
	Vector3 moveVec;			//移動ベクトル
	float moveSpeed;			//移動速度
	bool isGraund;				//接地しているかフラグ

	//------------ HP関連 ------------//
	bool isAlive;				//生きてるか否か
	int hp;						//現在のヒットポイント
	int maxHP;					//最大HP

	//------------ その他 ------------//
	ModelObj obj;				//オブジェクト

public:
	Charactor();
	virtual ~Charactor() {};
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	// ゲッター //
	//座標取得
	Vector3 GetPos()const { return obj.mTransform.position; }
	//大きさ取得
	Vector3 GetScale()const { return obj.mTransform.scale; }
	//生きてるかフラグ取得
	bool GetIsAlive()const { return isAlive; }

	// セッター //
	//座標設定
	void SetPos(const Vector3& pos) { obj.mTransform.position = pos; }
	//大きさ設定
	void SetScale(const Vector3& scale) { obj.mTransform.scale = scale; }
	//生きてるかフラグ設定
	void SetIsAlive(bool alive) { isAlive = alive; }

private:
	//コピー禁止
	Charactor& operator=(const Charactor&) = delete;
};

