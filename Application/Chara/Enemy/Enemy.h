#pragma once
#include "GameObject.h"
#include "EnemyState.h"
#include "Easing.h"
#include "WaxGroup.h"
#include "EnemyUI.h"
#include "Quaternion.h"
#include "Vector2.h"
#include "EnemyAttackState.h"
#include "EnemyBehaviorEditor.h"
#include "WaxVisual.h"

//このEnemyは基底にしたい
//ので、処理を分化して現状のタンクくんというクラスを別で作る
class Enemy : public GameObject
{
private:
	//------------ 移動関連 ------------//
	Vector3 moveVec;			//移動ベクトル
	float moveSpeed;			//移動速度
	float slowMag;				//減速率
	float slowCoatingMag;		//蝋かけられたときの減速率
	bool isGraund;				//接地しているかフラグ
	float gravity;				//重力

	Vector3 basis;
	Vector3 behaviorOrigen;

	//------------ 回転関連 ------------//
	Vector3 rotVec;				//回転ベクトル

	//---- ノックバック関連 ----//
	Vector3 knockbackVec;		//ノックバックする方向
	float knockbackSpeed;		//ノックバックさせる変数(速度に加算、タイマーに合わせて減少)
	float knockbackRange;		//ノックバックする距離
	Vector3 knockRadianStart;
	float knockRadianX;
	float knockRadianZ;

	Easing::EaseTimer knockbackTimer;	//ノックバックする時間

	//------------ 攻撃関連 ------------//
	bool isAttack;						//攻撃してるかフラグ
	float atkPower;						//攻撃力
	Easing::EaseTimer atkCoolTimer;		//攻撃するまでのクールタイム

	bool isEscape;						//脱出行動してるかフラグ
	float escapePower;					//蝋から脱出する力
	Easing::EaseTimer escapeCoolTimer;	//脱出行動のクールタイム

	std::string attackStateStr;		//状態を文字列で保存

	std::unique_ptr<EnemyAttackState> attackState;			//攻撃を管理するステート
	std::unique_ptr<EnemyAttackState> nextAttackState;		//攻撃を管理するステートの次ステート

	ModelObj attackDrawerObj;			//上記の当たり判定描画オブジェクト


	//------------ HP関連 ------------//
	float hp;				//現在のヒットポイント
	float maxHP;			//最大HP
	Easing::EaseTimer mutekiTimer;		//無敵時間さん
	int32_t waxSolidCount = 0; //ロウのかかり具合カウント
	int32_t requireWaxSolidCount = 10; //完全に固まるまでに必要なロウのかかり回数
	float waxShakeOffTimer = 0; //ロウを振り払うタイマー
	float requireWaxShakeOffTime = 5.0f; //ロウを振り払うまでにかかる時間(秒)

	Easing::EaseTimer whiteTimer = 0.5f;

	//------------ 出現関連 ------------//
	Easing::EaseTimer warningTimer = 1.0f;
	Easing::EaseTimer spawnTimer = 1.0f;

	Easing::EaseTimer warningRoop = 1.0f;
	Color warningColor = { 1,1,1,0 };

	//------------ その他 ------------//
	ModelObj* target = nullptr;
	//攻撃してきた対象
	ModelObj* attackTarget = nullptr;

	std::unique_ptr<EnemyState> state;			//状態管理
	std::unique_ptr<EnemyState> nextState;		//次のステート
	std::string stateStr;			//状態を文字列で保存

	EnemyUI ui;	//HP表示

	BehaviorData loadBehaviorData;
	std::string loadFileName = "";

	bool forceRot = false;

	//見た目用のロウ
	std::vector<std::unique_ptr<WaxVisual>> waxVisual;

public:
	Easing::EaseTimer solidTimer;	//動けなくなっている時間

	Vector3 shack;
	Color changeColor = { 1,1,1,1 };

	bool changingState = false;
	bool changingAttackState = false;

	//------attackState内で使用するやつ-----//
	Vector3 attackStartPos;
	Vector3 attackEndPos;

	float attackMovePower = 100.0f;	//移動量
	ModelObj predictionLine;	//予測線

	ColPrimitive3D::Sphere attackHitCollider;	//攻撃状態へ遷移する当たり判定

	//コレクトステート内で使用するタイマー(調整できるようにwaxが持つ)
	Easing::EaseTimer collectTimer;

	Vector3 collectPos;			//回収される場所
	bool isCollect;				//回収されてるか
	Vector3 oriScale;			//元の大きさ

	//配列にロードする
	std::vector<std::string> stateStrings;
	//今何番のステートを実行しているかの管理番号
	int32_t stateManageNumber = 0;

	//------ 識別関連 ------//
	std::string enemyTag = "";	//識別用に、生成時に敵のタグを入れる

protected:
	//今までのUpdateの中身(継承先で必ず呼ぶように)
	void BaseUpdate();

private:
	//ノックバック処理をまとめた
	void KnockBack();

	//回転する処理
	void KnockRota();
	//通常時の回転
	void Rotation(const Vector3& pVec);
	//Updateの最初で初期化するもの
	void Reset();

	void UpdateAttackCollider();
	void DrawAttackCollider();

public:
	Enemy(ModelObj* target_);
	virtual ~Enemy();
	void Init() override;
	void Update()override;	//ここではBaseUpdate
	void Draw() override;

	void TransfarBuffer();

	//引数があればノックバックもする(その方向を向かせるために攻撃対象も入れる)
	void DealDamage(uint32_t damage, const Vector3& dir, ModelObj* target_);

	//追いかける対象を変更
	void SetTarget(ModelObj* target_);

	ModelObj* GetTarget();

	//行動を初期化
	void BehaviorReset();

	void CreateWaxVisual(Vector3 spawnPos = { 0,0,0 });

	void WaxVisualUpdate();

	//ウェーブタイマーを開始
	void StartToMoving();

	/// <summary>
	/// 状態変更
	/// 内部で設定している優先度を見て、同値以上であれば遷移、未満であれば何もしない
	/// </summary>
	/// <typeparam name="ChangeEnemyState">変化先のEnemyState</typeparam>
	template <typename ChangeEnemyState>
	void ChangeState() {
		std::unique_ptr<EnemyState> change = std::make_unique<ChangeEnemyState>();
		//より優先度が高いステートであれば遷移準備
		if (state->GetPriority() <= change->GetPriority()) {
			changingState = true;
			nextState = std::make_unique<ChangeEnemyState>();
		}
	};

	template <typename ChangeEnemyAttackState>
	void ChangeAttackState() {
		std::unique_ptr<EnemyAttackState> change = std::make_unique<ChangeEnemyAttackState>();
		//より優先度が高いステートであれば遷移準備
		changingAttackState = true;
		nextAttackState = std::make_unique<ChangeEnemyAttackState>();
	};

	// ゲッター //
	//状態文字情報を取得
	std::string GetState() { return stateStr; }
	std::string GetAttackState() { return attackStateStr; }
	//脱出行動フラグ取得
	bool GetIsEscape() { return isEscape; }
	//蝋から脱出する力取得
	float GetEscapePower() { return escapePower; }
	//脱出行動のクールタイム取得
	Easing::EaseTimer* GetEscapeCoolTimer() { return &escapeCoolTimer; }
	//HPの取得
	float GetHP() { return hp; };
	float GetMaxHP() { return maxHP; };
	//固まってるかどうか
	bool GetIsSolid();
	//移動量
	Vector3 GetMoveVec() { return moveVec; };
	//スポーン時の位置を取得
	Vector3 GetOriginPos();
	//移動速度を取得
	float GetMoveSpeed() { return moveSpeed; };
	//出現済みかどうかを取得
	bool GetIsSpawn();

	// セッター //
	//減速率設定
	void SetSlowMag(float mag) { slowMag = mag; }
	//蝋かけられたときの減速率設定
	void SetSlowCoatingMag(float mag) { slowCoatingMag = mag; }
	//攻撃フラグ設定
	void SetIsAlive(bool flag) { isAlive = flag; }
	//攻撃力設定
	void SetAtkPower(float power) { atkPower = power; }
	//脱出行動フラグ設定
	void SetIsEscape(bool flag);
	//蝋から脱出する力設定
	void SetEscapePower(float power) { escapePower = power; }
	//状態文字情報を設定
	void SetStateStr(std::string str) { stateStr = str; }
	void SetAttackStateStr(std::string str) {attackStateStr = str; }
	//ノックバック距離を設定
	void SetKnockRange(float knockRange) { knockbackRange = knockRange; };
	//ノックバック時間を設定
	void SetKnockTime(float knockTime) { knockbackTimer.maxTime_ = knockTime; };
	//無敵時間さん!?を設定
	void SetMutekiTime(float mutekiTime) { mutekiTimer.maxTime_ = mutekiTime; };
	//移動速度を設定
	void SetMoveSpeed(float setSpeed) { moveSpeed = setSpeed; };
	//強制的に死亡させる
	void SetDeath();
	//行動ファイルを設定、読み込み
	void SetBehaviorOrder(const std::string& order);
	//敵の出現タイミングなど、敵の情報を設定
	void SetEnemyOrder(const std::string& order);
	//別の回転を使う場合に立てるフラグ
	void SetForceRot(bool forceRot_) { forceRot = forceRot_; };
	//移動量に足す
	void MoveVecPlus(const Vector3& plusVec);
	//回転量に足す
	void RotVecPlus(const Vector3& plusVec);
	//
	void BehaviorOrigenPosPlus(const Vector3& plusVec) { behaviorOrigen += plusVec; };
	void BehaviorOrigenReset() { behaviorOrigen = basis; }
};

