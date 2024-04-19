#pragma once
#include "GameObject.h"
#include "Easing.h"
#include "WaxState.h"
#include <vector>
#include "Vector3.h"

struct DisolveBuffer
{
	float disolveValue;
};

//蝋
class Wax : public GameObject
{
public:
	//------------ 攻撃関連 ------------//
	float atkSpeed;				//攻撃の射出速度
	float atkSize;				//攻撃範囲の大きさ
	Easing::EaseTimer atkTimer;	//攻撃時間計測用
	Vector3 startPos;	//初期地点
	Vector3 interPos;	//高さを入れた中間地点
	Vector3 endPos;		//終点

	//------------ 移動関連 -------//
	Vector3 moveVec;

	//------------ 燃焼関連 ------------//
	static Color waxOriginColor;		//蝋の元の色
	static Color waxEndColor;			//蝋の燃えた後の色
	
	Easing::EaseTimer igniteTimer;	//燃え始めて色が変わるの時間
	Easing::EaseTimer burningTimer;	//蝋が燃え尽きるまでの時間
	Easing::EaseTimer extinguishTimer;	//蝋が燃え尽きて消えるまでの時間
	
	std::unique_ptr<WaxState> state;			//燃焼の状態管理
	std::unique_ptr<WaxState> nextState;		//遷移先
	bool changingState = false;

	std::string stateStr;		//状態を文字列に保存用

	//------------ 固形関連 ------------//
	bool isSolid;					//固形かフラグ
	bool isSolided;					//1F前に固形だったかフラグ
	bool isFlashing;				//点滅フラグ

	//------------ その他 ------------//
	float gravity = 0.098f;		//重力
	bool isGround;				//接地してるかフラグ
	uint32_t groupNum;			//所属グループの要素番号
	float disolveValue;			//ディゾルブの強さ
	Vector3 collectPos;			//回収される場所

	Vector2 screenPos;
	Vector2 iconSize;
	Sprite minimapIcon;

	//コレクトステート内で使用するタイマー(調整できるようにwaxが持つ)
	Easing::EaseTimer collectTimer;

private:
	SRConstBuffer<DisolveBuffer> mDisolveBuff;

	std::vector<Vector3> spline;
public:
	Wax();
	//生成時に必要な情報だけもらって独立
	void Init(Transform transform, Vector3 endPos, float height, float size, float atkTime);
	//継承元との齟齬がないように空のやつを宣言だけしておく
	void Init()override {};
	
	void Update()override;
	void Draw()override;
	void DrawUI();

	Color SolidBling(const Easing::EaseTimer& timer);

	bool IsBurning();
	bool IsNormal();

	////ダメージ受ける
	//void Damage(float damage) { hp -= damage; }

	//状態変更
	template <typename ChangeWaxState>
	void ChangeState() {
		changingState = true;
		nextState = std::make_unique<ChangeWaxState>();
	};

	void DeadParticle();

	// ゲッター //
	//固まった瞬間かどうかを返す
	bool GetIsSolidNow();
	//状態文字情報を設定
	std::string GetState() { return stateStr; }

	// セッター //
	//状態文字情報を設定
	void SetStateStr(std::string str) { stateStr = str; }
};