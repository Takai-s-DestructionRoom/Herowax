#pragma once
#include "GameObject.h"
#include "SimpleParticle.h"
#include "RingParticle.h"
#include "Easing.h"
#include "PlayerState.h"
#include "PlayerUI.h"
#include "FireUnit.h"
#include "ColPrimitive3D.h"
#include <stdint.h>

class Boss;

class Player : public GameObject
{
public:
	//------------ 移動関連 ------------//
	Vector3 moveVec;			//移動ベクトル
	float moveSpeed;			//移動速度
	float moveAccel;			//移動加速度
	float moveAccelAmount;		//移動加速度の1Fあたりの加算量
	bool isGround;				//接地しているかフラグ
	bool isJumping;					//ジャンプ中かフラグ
	Easing::EaseTimer jumpTimer;	//ジャンプ時間計測用
	float jumpHeight;				//ジャンプしてる高さ
	float maxJumpHeight;			//ジャンプの最高到達点
	float jumpPower;				//ジャンプ力(初速)
	float jumpSpeed;				//ジャンプ速度
	float gravity = 0.098f;			//重力
	Vector3 stickVec = { 0,0,1 };	//正面ベクトル
	Vector3 initPos = { 0,0,0 };	//初期位置
	Vector3 initRot = { 0,0,0 };	//初期向き

	//------------ 回転関連 ------------//
	Vector3 rotVec;				//回転ベクトル
	Vector3 oldRot;				//回転ベクトル

	Easing::EaseTimer backwardTimer;	//のけぞり管理

	//------------ HP関連 ------------//
	float hp;				//現在のヒットポイント
	float maxHP;			//最大HP

	Easing::EaseTimer damageCoolTimer;	//再びダメージくらうようになるまでのクールタイム

	//------------ 攻撃関連 ------------//
	bool isAttack;					//攻撃中かフラグ
	Vector3 atkVec;					//攻撃方向
	float atkHeight;				//攻撃する高さ
	float atkSpeed;					//攻撃の射出速度
	float atkRange;					//攻撃範囲
	float atkSize;					//攻撃範囲の大きさ
	int32_t atkPower;				//攻撃力
	Easing::EaseTimer atkCoolTimer;	//攻撃のクールタイム
	Easing::EaseTimer atkTimer;		//攻撃時間計測用
	Easing::EaseTimer solidTimer;	//固まるまでの時間(調整用)

	bool isMugenAttack = false;		//攻撃中でも次の攻撃を出せるフラグ

	//----------- ロウ回収関連 ------------//
	int32_t waxStock;			//ロウストック
	int32_t maxWaxStock;		//ロウストック最大値
	bool isWaxStock;			//ストック性にするかフラグ
	bool isCollectFan;			//回収範囲扇型にするかフラグ
	bool isCollect;				//回収できるかフラグ

	float waxCollectRange;				//ロウ回収するレイの範囲(横幅)
	ColPrimitive3D::Ray collectCol;		//ロウ回収する範囲当たり判定
	ModelObj collectRangeModel;			//ロウ回収範囲描画用
	float waxCollectVertical;			//ロウ回収するレイの範囲(縦幅)

	float waxCollectDist;					//ロウ回収する扇の距離
	float waxCollectAngle;					//ロウ回収する扇の角度(180°以内)
	ColPrimitive3D::Sphere collectColFan;	//ロウ回収する範囲当たり判定
	ModelObj collectRangeModelCircle;		//ロウ回収範囲描画用
	ModelObj collectRangeModelRayLeft;			//ロウ回収範囲描画用
	ModelObj collectRangeModelRayRight;			//ロウ回収範囲描画用

	int32_t waxCollectAmount;			//ロウ回収量
	
	//----------- 挑発関連 ------------//
	bool isTauntMode = false;		//ロウを直接当てると敵の追いかける対象が自分に変わるモード

	//----------- 新攻撃(パブロ攻撃)関連 -------------//
	float pabloRange;			//最大射程
	float pabloSideRange;		//横の射程
	float pabloSeparator;		//射程の分割数
	float pabloSpeedMag;		//パブロ攻撃時の移動速度
	float shotDeadZone = 1.0f;
	float pabloShotSpeedMag = 2.f;	//プレイヤーの正面に出すための係数
	int32_t waxNum = 5;				//一度に出るロウの数

	//------------ 炎関係 ------------//
	FireUnit fireUnit;
	bool isFireStock;			//炎をストック性にするかフラグ

	//------------ ☆MUTEKI☆関連 ------------//
	Easing::EaseTimer godmodeTimer;	//無敵の制限時間(デフォ10秒)
	bool isGodmode;					//無敵かフラグ

	//ゲーミングパレットの遷移タイマー
	const float kGamingTimer_ = 0.3f;
	Easing::EaseTimer redTimer_ = kGamingTimer_;
	Easing::EaseTimer greenTimer_ = kGamingTimer_;
	Easing::EaseTimer blueTimer_ = kGamingTimer_;

	//------------ その他 ------------//
	std::unique_ptr<PlayerState> attackState;
	std::unique_ptr<PlayerState> nextState;
	bool changingState = false;

	ColPrimitive3D::Sphere attackHitCollider;	//敵が攻撃状態へ遷移する当たり判定
	
	PlayerUI ui;

private:
	ModelObj attackDrawerObj;			//上記の当たり判定描画オブジェクト

	Easing::EaseTimer blinkTimer;

	float blinkNum = 10;		//点滅回数

	Boss* boss = nullptr;

public:
	Player();

	void Init()override;
	void Update()override;
	void Draw()override;

	//移動関数
	void MovePad();
	void MoveKey();

	//回転関数
	void Rotation();

	//攻撃(ステートに移動)
	void Attack();

	//パブロ攻撃処理のまとめ
	void PabloAttack();

	//ロウを回収
	void WaxCollect();

	//ゲーミングカラーの更新処理
	//ゲーミングカラー返す
	Color GamingColorUpdate();

	//状態変更
	template <typename ChangePlayerState>
	void ChangeState() {
		nextState = std::make_unique<ChangePlayerState>();
		changingState = true;
	};

	//ボスを参照するために入れる
	void SetBoss(Boss* boss_) {boss = boss_;};

	// ゲッター //
	//お試し実装:殴った相手が自分を追っかけてくるモード
	bool GetTauntMode() { return isTauntMode; };
	//足元の座標取得
	Vector3 GetFootPos();

	// セッター //
	//回収できるかフラグ設定
	void SetIsCollect(bool frag) { isCollect = frag; }
	//無敵状態設定
	void SetIsGodmode(bool frag) { isGodmode = frag; }

	//ダメージを与える
	void DealDamage(uint32_t damage);

private:
	void DamageBlink();	//被弾時の点滅(後々もっとリッチなのに置き換え予定)

	void Reset();	//Updateの最初で初期化するもの

	void UpdateAttackCollider();
	void DrawAttackCollider();
};