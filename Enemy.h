#pragma once
#include "ModelObj.h"

class Enemy
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

	//追跡する対象(プレイヤーを入れる)
	ModelObj* target = nullptr;

public:
	Enemy(ModelObj* target_);
	void Init();
	void Update();
	void Draw();

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
	Enemy& operator=(const Enemy&) = delete;
};

