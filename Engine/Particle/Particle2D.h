#pragma once
#include "Transform.h"
#include "LightGroup.h"
#include "SRConstBuffer.h"
#include "SRVertexBuffer.h"
#include "VertexBuffer.h"
#include "ViewProjection.h"
#include "Easing.h"
#include "Util.h"

// 2Dのパーティクル //
//粒子1粒
struct Particle2D
{
	//座標
	Vector2 pos;
	Vector2 startPos;
	//大きさ
	float scale;
	float startScale;	//開始時の大きさ
	float endScale;		//終了時の大きさ
	//角度
	float rot;
	float plusRot;	//更新処理で回転させるときに使う用
	//速度
	Vector2 velo;
	//加速度
	Vector2 accel;
	//重力
	float gravity = 0.098f;

	//色
	Color color;
	//生存時間
	Easing::EaseTimer aliveTimer;

	//イージング用タイマー
	Easing::EaseTimer easeTimer = 1.0f;
	Easing::EaseTimer growingTimer = 0.1f;

	//エミッター座標からの距離
	float radius = 0.f;
	float startRadius = 0.f;	//開始時の距離
	float endRadius = 0.f;		//終了時の距離
};

// エミッター //
class IEmitter2D
{
protected:
	Transform transform;
	SRConstBuffer<TransformBuffer> transformBuff;
	SRConstBuffer<ViewProjectionBuffer> viewProjectionBuff;

	//頂点群
	std::vector<VertexParticle2D> vertices;
	SRVertexBuffer vertBuff;

	float minScale_;	//パーティクルの最小サイズ
	float maxScale_;	//パーティクルの最大サイズ
	//元の大きさ
	Vector2 originalScale_;
	//拡縮用倍率
	float scaling_;
	//拡縮用タイマー
	Easing::EaseTimer scalingTimer_ = 1.0f;
	//パーティクル出すタイミング用タイマー
	uint32_t particleTimer_ = 0;
	//何フレームに一回パーティクル追加するか
	uint32_t addInterval_;

	//経過時間のスピード(スローモーション用)
	bool isElapse_ = true;					//経過時間の影響受けるかフラグ
	float elapseSpeed_;

	const uint32_t maxParticle_ = 64;		//最大数
	std::vector<Particle2D> particles_;		//パーティクル配列

	bool isActive_ = true;					//有効にするかフラグ

	bool isGravity_ = false;				//重力の影響受けるかフラグ

	Texture texture;						//割り当てるテクスチャ

public:
	//コンストラクタ
	IEmitter2D();
	//デストラクタ
	virtual ~IEmitter2D() = default;

	//初期化
	//このままの処理を呼びたいなら継承先でこれを呼ぶ
	virtual void Init();
	//更新(共通部分)
	//このままの処理を呼びたいなら継承先でこれを呼ぶ
	virtual void Update();
	//描画
	void Draw();

	//定数バッファへ送信
	void TransferBuffer(ViewProjection viewprojection);

	//パーティクル追加(固有処理にしたかったらoverrideで上書きする)
	//life:秒数指定なので注意
	virtual void Add(uint32_t addNum, float life, Color color, TextureHandle tex, float minScale, float maxScale,
		Vector2 minVelo, Vector2 maxVelo, float accelPower = 0.f, float minRot = 0.f,float maxRot = 0.f,
		float growingTimer = 0.f, float endScale = 0.f, bool isGravity = false, float rejectRadius = 0.f);

	////リング状パーティクル追加(固有処理にしたかったらoverrideで上書きする)
	////life:秒数指定なので注意
	//virtual void AddRing(uint32_t addNum, float life, Color color, TextureHandle tex,
	//	float startRadius, float endRadius, float minScale, float maxScale,
	//	float minVeloY, float maxVeloY, Vector2 minRot = {}, Vector2 maxRot = {},
	//	float growingTimer = 0.f, float endScale = 0.f, bool isGravity = false, bool isBillboard = false);

	//パーティクル全消し
	void ClearParticles() { particles_.clear(); }

	//ゲッター//
	//座標取得
	Vector2 GetPos()const { return transform.position; }
	//大きさ取得
	Vector2 GetScale()const { return transform.scale; }
	//パーティクル全部死んだか取得
	bool GetParticlesDead()const { return particles_.empty(); }
	//何個パーティクルあるか取得
	size_t GetParticlesSize()const { return particles_.size(); }
	//有効フラグ取得
	bool GetIsActive()const { return isActive_; }

	//セッター//
	//座標設定
	void SetPos(float x, float y, float z) { transform.position = { x,y,z }; }
	//座標設定
	void SetPos(const Vector2& pos) { transform.position = pos; }
	//大きさ設定
	void SetScale(const Vector2& scale);
	////角度設定
	//void SetRot(float rot) { rot_ = rot; }
	//有効フラグ設定
	void SetIsActive(bool isActive) { isActive_ = isActive; }
	//重力フラグ設定
	void SetIsGravity(bool isGravity) { isGravity_ = isGravity; }

	//拡縮用タイマーが切り替わる時間設定(秒)
	void SetScalingTimer(float timer);
	//拡縮用タイマー開始
	//isRun:true = プラスされてく,false:マイナスされてく
	void StartScalingTimer(bool isRun = true);

	//経過時間スピード設定
	void SetElapseSpeed(const float elapseSpeed) { elapseSpeed_ = elapseSpeed; }
	//拡縮用タイマー開始
	void SetIsElapse(bool isElapse) { isElapse_ = isElapse; }
};