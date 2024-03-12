#pragma once
#include "GameObject.h"
#include "Easing.h"
#include "WaxState.h"

//蝋
class Wax : public GameObject
{
public:
	//------------ 攻撃関連 ------------//
	uint32_t atkPower;			//攻撃力
	Vector3 atkVec;				//攻撃方向
	float atkSpeed;				//攻撃の射出速度
	Vector2 atkRange{};			//攻撃範囲
	float atkSize;				//攻撃範囲の大きさ
	Easing::EaseTimer atkTimer;	//攻撃時間計測用

	//------------ 燃焼関連 ------------//
	Color waxOriginColor;		//蝋の元の色
	Color waxEndColor;			//蝋の燃えた後の色
	
	Easing::EaseTimer igniteTimer;	//燃え始めて色が変わるの時間
	Easing::EaseTimer burningTimer;	//蝋が燃え尽きるまでの時間
	Easing::EaseTimer extinguishTimer;	//蝋が燃え尽きて消えるまでの時間
	
	std::unique_ptr<WaxState> state;			//燃焼の状態管理
	std::string stateStr;		//状態を文字列に保存用

	//------------ 固形関連 ------------//
	bool isSolid;					//固形かフラグ
	bool isSolided;					//1F前に固形だったかフラグ
	Easing::EaseTimer solidTimer;	//固形になるまでの時間
	bool isFlashing;				//点滅フラグ

	//------------ その他 ------------//
	float gravity = 0.098f;		//重力
	bool isGround;				//接地してるかフラグ
	uint32_t groupNum;			//所属グループの要素番号

public:
	Wax();
	//生成時に必要な情報だけもらって独立
	void Init(
		uint32_t power, Vector3 vec,float speed,
		Vector2 range, float size, float atkTime, float solidTime);
	//継承元との齟齬がないように空のやつを宣言だけしておく
	void Init()override {};
	
	void Update()override;
	void Draw()override;

	bool IsBurning();
	bool IsNormal();

	////ダメージ受ける
	//void Damage(float damage) { hp -= damage; }

	//状態変更
	template <typename ChangeWaxState>
	void ChangeState() {
		state = std::make_unique<ChangeWaxState>();
	};

	// ゲッター //
	//固まった瞬間かどうかを返す
	bool GetIsSolidNow();
	//状態文字情報を設定
	std::string GetState() { return stateStr; }

	// セッター //
	//状態文字情報を設定
	void SetStateStr(std::string str) { stateStr = str; }
};