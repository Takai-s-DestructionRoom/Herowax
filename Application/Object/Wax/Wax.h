#pragma once
#include "../../Chara/Charactor/Charactor.h"
#include "Easing.h"

//蝋
class Wax
{
public:
	//------------ 攻撃関連 ------------//
	uint32_t atkPower;			//攻撃力
	Vector3 atkVec;				//攻撃方向
	Vector3 atkOriginPos;		//攻撃の出現位置
	float atkDist;				//攻撃の射出距離
	Vector2 atkRange{};			//攻撃範囲
	float atkSize;				//攻撃範囲の大きさ
	Easing::EaseTimer atkTimer;	//攻撃時間計測用

	//------------ その他 ------------//
	ModelObj obj;				//オブジェクト
	bool isAlive;				//生存フラグ

public:
	Wax();
	//生成時に必要な情報だけもらって独立
	void Init(
		uint32_t power, Vector3 vec,Vector3 originPos,
		float dist,Vector2 range, float size,float time);
	void Update();
	void Draw();

	// ゲッター //
	//座標取得
	Vector3 GetPos()const { return obj.mTransform.position; }
	//大きさ取得
	Vector3 GetScale()const { return obj.mTransform.scale; }
	//生存フラグ取得
	bool GetIsAlive()const { return isAlive; }

	// セッター //
	//座標設定
	void SetPos(const Vector3& pos) { obj.mTransform.position = pos; }
	//大きさ設定
	void SetScale(const Vector3& scale) { obj.mTransform.scale = scale; }
	//生きてるかフラグ設定
	void SetIsAlive(bool alive) { isAlive = alive; }
};