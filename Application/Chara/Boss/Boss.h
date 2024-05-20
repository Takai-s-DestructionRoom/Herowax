#pragma once
#include "GameObject.h"
#include "Easing.h"
#include "Quaternion.h"
#include "Vector2.h"
#include "BossState.h"
#include "BossUI.h"
#include "BossAI.h"
#include "BossPart.h"
#include <array>

enum class PartsNum
{
	RightHand,	//1
	LeftHand,	//0

	Max			//2
};

class Boss : public GameObject
{
private:
	//------------ 移動関連 ------------//
	Vector3 moveVec;			//移動ベクトル
	float moveSpeed;			//移動速度
	float oriSize;				//元の大きさ比率

	//------------ HP関連 ------------//
	float hp;				//現在のヒットポイント
	float maxHP;			//最大HP
	Easing::EaseTimer mutekiTimer;			//無敵時間さん

	//------------ その他 ------------//
	std::unique_ptr<BossState> nextState;	//次のステート
	std::string stateStr;					//状態を文字列で保存

	ModelObj* target = nullptr;				//攻撃対象

	BossUI ui;
	BossAI ai;

	bool changingState = false;		//ステート変更中かフラグ
	bool isDrawObj = true;			//オブジェクト描画するかフラグ

	float phaseTimer;				//フェーズ移行に使うタイマー

	ModelObj barrier;						//ダメージ与えられないの伝える用バリア
	bool isBarrier;							//バリア張られてるかフラグ
	Easing::EaseTimer barrierCrushTimer;	//バリア破壊される時用タイマー

	//------------- イベントシーン関連 -----------//
	Easing::EaseTimer bossSpawnTimer;
	//撃破演出呼び出し用フラグ
	bool deadEventCall = false;

	bool isOldBodySolid = false;	//前フレームで本体が固まっているか

	//---------被弾時表現関連------------//
	Easing::EaseTimer whiteTimer;	//被弾時に白く光るのを管理する

	//---------固まり表現関連---------//
	int32_t waxSolidCount = 0; //ロウのかかり具合カウント
	int32_t requireWaxSolidCount = 10; //完全に固まるまでに必要なロウのかかり回数
	Easing::EaseTimer waxShakeOffTimer = 5.0f; //ロウを振り払うタイマー
	float armRemainOffTime = 0.1f; //両腕が残ってるときのロウの振り払い時間
	float armBreakOffTime = 5.0f; //両腕がないときのロウの振り払い時間

	Easing::EaseTimer shakeTimer = 0.5f;
	Easing::EaseTimer waxScatterTimer = 0.1f;
	Vector3 shake{};
	float shakePower;

	//ロウを出すためのパラメータ(適当でもいい)
	float solidTime = 0;
	int32_t atkPower = 0;
	float atkRange = 0;
	float atkSize = 0;
	float atkSpeed = 0;
	float atkTime = 0;

public:
	std::unique_ptr<BossState> state;	//状態管理

	std::array<Parts, 2> parts;			//体のパーツ
	ModelObj targetCircle;				//攻撃位置表示用

	//------------ 行動関連 ------------//
	Easing::EaseTimer standTimer;		//待機時間
	Easing::EaseTimer punchTimer;		//パンチにかかる時間
	Easing::EaseTimer punchStayTimer;	//パンチ終わってからモーション終了までの時間
	bool isAppearance = false;			//出現中かフラグ
	bool isDead = false;				//撃破演出中かフラグ

	std::array<PaintableModelObj, 32> fallParts;	//落ちてくるパーツ
	std::array<ModelObj, 32> warning;			//警告
	Easing::EaseTimer fallAtkShoutTimer;//落下攻撃前の咆哮時間
	Easing::EaseTimer fallAtkTimer;		//落下攻撃にかかる時間
	Easing::EaseTimer fallAtkStayTimer;	//落下攻撃終わってからモーション終了までの時間

	//------------ 吸収対象の位置 ------//
	Vector3 collectPos{};
	
private:
	//すべてのステートでの共通処理
	void AllStateUpdate();

public:
	static Boss* GetInstance();

	Boss();
	virtual ~Boss();
	void Init() override;
	void Update() override;
	void Draw() override;

	void Shake(float time,float power);

	//状態変更
	template <typename ChangePlayerState>
	void ChangeState() {
		nextState = std::make_unique<ChangePlayerState>();
		changingState = true;
	};

	// ゲッター //
	//状態文字情報を取得
	std::string GetStateStr() { return stateStr; }
	//攻撃対象を取得
	ModelObj* GetTarget() { return target; }
	//元の大きさ比率取得
	float GetOriSize() { return oriSize; }
	/// <summary>
	/// 固まっているかどうかを取得
	/// </summary>
	/// <param name="num">どの部位の状態を取得するか maxを指定するとボスの体を取得</param>
	/// <returns>true or false</returns>
	bool GetIsSolid(PartsNum num);

	//本体だけが残っているか取得
	bool GetIsOnlyBody();

	// セッター //
	//状態文字情報を設定
	void SetStateStr(std::string str) { stateStr = str; }
	//攻撃対象を設定
	void SetTarget(ModelObj* target_) { target = target_; }

	//ダメージを与える
	void DealDamage(int32_t damage);
};