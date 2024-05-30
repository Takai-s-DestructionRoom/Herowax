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
#include "CollectPart.h"
#include "WaxUI.h"
#include "WaxWall.h"
#include "BonusUI.h"

class Boss;

class AfterImage : public GameObject
{
public:
	void Init()override;
	void Update()override;
	void Draw()override;
private:
	Easing::EaseTimer lifeTimer = 10.f;
};

class Player : public GameObject
{
public:
	//------------ 移動関連 ------------//
	Vector3 moveVec;			//移動ベクトル
	float moveSpeed;			//移動速度
	float moveAccel = 0;		//移動加速度
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
	bool isMove;					//行動可能かフラグ
	Vector3 slideVec;				//壁に当たった時スライドさせるベクトル
	float toWallLen;				//壁との距離

	//------------ 回転関連 ------------//
	Vector3 rotVec;				//回転ベクトル
	Vector3 oldRot;				//回転ベクトル

	Easing::EaseTimer backwardTimer;	//のけぞり管理

	//------------ HP関連 ------------//
	float hp;				//現在のヒットポイント
	float maxHP;			//最大HP
	float oldHp;			//1フレーム前のHP

	Easing::EaseTimer damageCoolTimer;	//再びダメージくらうようになるまでのクールタイム

	//------------ 攻撃関連 ------------//
	bool isAttack;					//攻撃中かフラグ
	float minRange;			//最大射程
	float maxRange;			//最大射程
	float atkSize;
	Vector3 atkVec;					//攻撃方向
	float atkHeight;				//攻撃する高さ
	int32_t atkPower;				//攻撃力
	int32_t involvePower = 1;		//巻き込み攻撃力
	Easing::EaseTimer atkCoolTimer;	//攻撃のクールタイム
	Easing::EaseTimer atkTimer;		//攻撃時間計測用
	Easing::EaseTimer solidTimer;	//固まるまでの時間(調整用)

	bool isMugenAttack = false;		//攻撃中でも次の攻撃を出せるフラグ

	//----------- ロウ回収関連 ------------//
	int32_t waxStock;			//ロウストック
	int32_t maxWaxStock;		//ロウストック最大値
	int32_t initWaxStock;		//ロウストック最大値(初期値)
	bool isWaxStock;			//ストック性にするかフラグ
	bool isCollect;				//回収できるかフラグ

	float waxCollectRange;				//ロウ回収するレイの範囲(横幅)
	ColPrimitive3D::Ray collectCol;		//ロウ回収する範囲当たり判定
	ModelObj collectRangeModel;			//ロウ回収範囲描画用
	float waxCollectVertical;			//ロウ回収するレイの範囲(縦幅)

	int32_t waxCollectAmount;			//ロウ回収量
	float waxCollectingTime = 0; //ロウ回収中の経過時間
	
	//----------- 新攻撃(パブロ攻撃)関連 -------------//
	float pabloSideRange;		//横の射程
	float pabloSeparator;		//射程の分割数
	float pabloSpeedMag;		//パブロ攻撃時の移動速度
	float shotDeadZone = 1.0f;
	float pabloShotSpeedMag = 2.f;	//プレイヤーの正面に出すための係数
	int32_t waxNum = 5;				//一度に出るロウの数

	//----------- 回避関連 -------------//
	Easing::EaseTimer avoidTimer = 0.5f;
	Easing::EaseTimer avoidConsumTimer = 0.1f;
	Vector3 avoidVec = {};
	int32_t avoidConsumWax = 5;	//回避時のロウの消費量
	float avoidSpeed = 0.0f;
	float maxAvoidSpeed = 1.5f;
	float minAvoidSpeed = 1.0f;

	//std::vector<std::unique_ptr<AfterImage>> afterimagesObj;

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

	std::vector<CollectPart*> carryingParts;
	
	//------------ その他 ------------//
	std::unique_ptr<PlayerState> attackState;
	std::unique_ptr<PlayerState> nextState;
	bool changingState = false;
	Vector3 cameraDir;				//カメラの向いてる方向

	ColPrimitive3D::Sphere attackHitCollider;	//敵が攻撃状態へ遷移する当たり判定
	
	PlayerUI ui;

	bool soundFlag = false;
	
	float modelChangeOffset;//モデルのズレに合わせたオフセット
	bool modelChange = false;

private:
	ModelObj attackDrawerObj;			//上記の当たり判定描画オブジェクト

	Easing::EaseTimer blinkTimer;

	float blinkNum = 10;		//点滅回数

	Boss* boss = nullptr;

	Color defColor;

	//上に配置してる人型オブジェクト系
	ModelObj humanObj;
	float humanOffset = 0.f;
	float humanScale = 0.0f;
	//モデル変更時に使うやつら
	float bagScale = 0.0f;
	float collectScale = 0.0f;

	//タンク内のロウ描画
	ModelObj tankWaterObj;
	float tankRadius = 1.0f;
	float tankValue = 0;
	struct TankWaterData {
		Vector3 centerPos;
		float upper;
		float amplitude;
		float frequency;
		float time;
	};
	SRConstBuffer<TankWaterData> tankBuff;
	ModelObj tankMeterObj;
	struct TankMeterData {
		Vector3 centerPos;
		float upper;
		float thickness;
	};
	SRConstBuffer<TankMeterData> tankMeterBuff;

	Vector3 modelOffset;//描画位置をずらす

	WaxUI waxUI;

	WaxWall waxWall;

	int32_t collectCount = 0;

public:
	Player();

	void Init()override;
	void Update()override;
	void Draw()override;

	//移動関数
	void MovePad();
	void MoveKey();

	//回避
	void Avoidance();

	//回転関数
	void Rotation();

	//パブロ攻撃処理のまとめ
	void PabloAttack();

	//ロウを回収
	void WaxCollect();

	//壁を作る
	void ShieldUp();

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
	//足元の座標取得
	Vector3 GetFootPos();
	//攻撃力を取得
	int32_t GetAttackPower() { return atkPower; };
	//ロウに巻き込んだ時の攻撃力を取得
	int32_t GetInvolvePower() { return involvePower; };

	bool GetWaxCollectButtonDown();

	bool GetDamageTrigger() { return oldHp != hp; };	//ダメージを受けた瞬間か

	// セッター //
	//回収できるかフラグ設定
	void SetIsCollect(bool frag) { isCollect = frag; }
	//無敵状態設定
	void SetIsGodmode(bool frag) { isGodmode = frag; }

	//ダメージを与える
	void DealDamage(float damage);

	void MaxWaxPlus(int32_t plus);

	void WaxLeakOut(int32_t leakNum, float minLeakLength = -2.5f, float maxLeakLength = 2.5f);

	void Reset();	//Updateの最初で初期化するもの

	WaxWall* GetWaxWall() {
		if (waxWall.isAlive) {
			return &waxWall;
		}
		else {
			return nullptr;
		}
	};

private:
	void DamageBlink();	//被弾時の点滅(後々もっとリッチなのに置き換え予定)


	void UpdateAttackCollider();
	void DrawAttackCollider();

	RootSignature* GetTankWaterRootSig();
	GraphicsPipeline* GetTankWaterPipeline();
	GraphicsPipeline* GetTankWaterPipelineB();

	RootSignature* GetTankMeterRootSig();
	GraphicsPipeline* GetTankMeterPipeline();
};