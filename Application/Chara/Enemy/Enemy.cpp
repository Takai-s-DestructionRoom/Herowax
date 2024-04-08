#include "Enemy.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "ParticleManager.h"
#include "Parameter.h"
#include "Quaternion.h"
#include "RImGui.h"
#include <TimeManager.h>

Enemy::Enemy(ModelObj* target_) : GameObject(),
moveSpeed(0.1f), slowMag(0.8f),
isGraund(true), hp(0), maxHP(10.f),
isEscape(false), escapePower(0.f), escapeCoolTimer(1.f),
isAttack(false), atkPower(0.f), atkCoolTimer(1.f),
gravity(0.2f), groundPos(0)
{
	state = std::make_unique<EnemyNormal>();
	attackState = std::make_unique<EnemyNonAttackState>();
	nextState = nullptr;
	nextAttackState = nullptr;
	obj = PaintableModelObj(Model::Load("./Resources/Model/firewisp/firewisp.obj", "firewisp", true));
	obj.mPaintDissolveMapTex = TextureManager::Load("./Resources/DissolveMap.png", "DissolveMapTex");
	target = target_;

	std::map<std::string, std::string> extract = Parameter::Extract("Enemy");
	knockbackTimer.maxTime_ = Parameter::GetParam(extract, "ノックバックにかかる時間", 0.5f);
	knockbackRange = Parameter::GetParam(extract, "ノックバックする距離", 0.5f);

	mutekiTimer.maxTime_ = Parameter::GetParam(extract, "無敵時間さん", 0.1f);

	obj.mTransform.scale = { 3,3,3 };
	attach = ModelObj(Model::Load("./Resources/Model/WaxAttach/WaxAttach.obj", "WaxAttach", true));
}

Enemy::~Enemy()
{
	//死んだときパーティクル出す
	ParticleManager::GetInstance()->AddSimple(
		obj.mTransform.position, obj.mTransform.scale * 0.5f, 10, 0.3f,
		Color::kGreen, "", 0.5f, 0.8f,
		{ -0.3f,-0.3f,-0.3f }, { 0.3f,0.3f,0.3f },
		0.05f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f);
}

void Enemy::Init()
{
	hp = maxHP;
}

void Enemy::Update()
{
	moveVec.x = 0;
	moveVec.z = 0;

	//ステートに入る前に、前フレームに足したシェイクを引いて元に戻す
	obj.mTransform.position -= shack;
	//シェイクを元に戻す
	shack = { 0,0,0 };

	//かかっているロウを振り払う
	if (waxSolidCount > 0 && waxSolidCount < requireWaxSolidCount) {
		waxShakeOffTimer += TimeManager::deltaTime;
		if (waxShakeOffTimer >= requireWaxShakeOffTime) {
			waxShakeOffTimer -= requireWaxShakeOffTime;
			waxSolidCount--;
		}
	}
	else {
		waxShakeOffTimer = 0;
	}

	//固まっていなければする処理
	if (waxSolidCount < requireWaxSolidCount) {
		//各ステート時の固有処理
		state->Update(this);	//移動速度に関係するので移動の更新より前に置く


		//前のステートと異なれば
		if (changingState) {
			//ステートを変化させる
			std::swap(state, nextState);
			changingState = false;
			nextState = nullptr;
		}

		attackState->Update(this);
		//前のステートと異なれば
		if (changingAttackState) {
			//ステートを変化させる
			std::swap(attackState, nextAttackState);
			changingAttackState = false;
			nextAttackState = nullptr;
		}
	}

	hp = Util::Clamp(hp, 0.f, maxHP);

	if (hp <= 0) {
		//hpが0になったら、自身の状態を固まり状態へ遷移
		ChangeState<EnemyAllStop>();
	}

	//プレイヤーに向かって移動するAI
	Vector3 pVec = target->mTransform.position - obj.mTransform.position;
	pVec.Normalize();
	pVec.y = 0;

	//無敵時間さん!?の更新
	mutekiTimer.Update();

	//ノックバックとかのけぞりとか
	KnockBack(pVec);

	//ノックバック中でないなら重力をかける
	if (!knockbackTimer.GetRun()) {
		//重力をかける
		moveVec.y -= gravity;
	}

	//減速率は大きいほどスピード下がるから1.0から引くようにしてる
	moveVec += pVec * moveSpeed *
		(1.f - slowMag) * (1.f - slowCoatingMag);

	//シェイクの値
	moveVec += shack;

	//座標加算
	obj.mTransform.position += moveVec;

	//地面座標を下回るなら戻す
	if (obj.mTransform.position.y <= groundPos) {
		obj.mTransform.position.y = groundPos;
		moveVec.y = 0;
	}

	obj.mTuneMaterial.mColor = changeColor;

	UpdateCollider();

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	obj.mPaintDataBuff->dissolveVal = waxSolidCount >= requireWaxSolidCount ? 1.0f : 0.3f / (requireWaxSolidCount - 1) * waxSolidCount;
	obj.mPaintDataBuff->color = Color(0.8f, 0.6f, 0.35f, 1.0f);
	obj.mPaintDataBuff->slide += TimeManager::deltaTime;
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);

	ui.Update(this);

	attach.mTransform.position = obj.mTransform.position;

	attach.mTransform.UpdateMatrix();
	attach.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Enemy::Draw()
{
	if (isAlive)
	{
		obj.Draw();
		ui.Draw();

		if (isDrawCollider) {
			DrawCollider();
		}

		/*if (GetState() == "AllStop") {
			attach.Draw();
		}*/
	}
}

void Enemy::KnockBack(const Vector3& pVec)
{
	//ノックバック時間の更新
	knockbackTimer.Update();

	//ノックバックを加算
	moveVec += knockbackVec * knockbackSpeed;

	//ノックバックを減少
	knockbackSpeed = Easing::OutQuad(knockbackRange, 0, knockbackTimer.GetTimeRate());

	//攻撃されたら
	if (attackTarget) {
		//プレイヤーに向かって移動するAI
		Vector3 aVec = attackTarget->mTransform.position - obj.mTransform.position;
		aVec.Normalize();
		aVec.y = 0;

		//ターゲットの方向を向いてくれる
		Quaternion aLookat = Quaternion::LookAt(aVec);
		////モデルが90度ずれた方向を向いているので無理やり修正(モデル変更時にチェック)
		//aLookat *= Quaternion::AngleAxis({ 0,1,0 }, -Util::PI / 2);

		//喰らい時のモーション遷移
		knockRadianX = Easing::InQuad(knockRadianStart.x, 0, knockbackTimer.GetTimeRate());
		knockRadianZ = Easing::InQuad(knockRadianStart.z, 0, knockbackTimer.GetTimeRate());

		Quaternion knockQuaterX = Quaternion::AngleAxis({ 1,0,0 }, knockRadianX);
		Quaternion knockQuaterZ = Quaternion::AngleAxis({ 0,0,1 }, knockRadianZ);

		//計算した向きをかける
		aLookat = aLookat * knockQuaterX * knockQuaterZ;

		//euler軸へ変換
		obj.mTransform.rotation = aLookat.ToEuler();
		//ノックバックが終わったら
		if (knockbackTimer.GetEnd()) {
			//ターゲットを解除
			attackTarget = nullptr;
		}
	}
	else
	{
		//普段はターゲットの方向を向く
		//ターゲットの方向を向いてくれる
		Quaternion pLookat = Quaternion::LookAt(pVec);
		////モデルが90度ずれた方向を向いているので無理やり修正(モデル変更時にチェック)
		//pLookat *= Quaternion::AngleAxis({ 0,1,0 }, -Util::PI / 2);

		//euler軸へ変換
		obj.mTransform.rotation = pLookat.ToEuler();
	}
}

void Enemy::SetTarget(ModelObj* target_)
{
	target = target_;
}

void Enemy::SetIsEscape(bool flag)
{
	isEscape = flag;
	//抜け出そうとしてる時ピンクに
	if (isEscape)
	{
		obj.mTuneMaterial.mColor = Color::kPink;
	}
	else
	{
		obj.mTuneMaterial.mColor = Color::kWhite;
	}
}

void Enemy::DealDamage(uint32_t damage)
{
	hp -= damage;
}

void Enemy::DealDamage(uint32_t damage, const Vector3& dir, ModelObj* target_)
{
	//無敵時間さん!?中なら攻撃を喰らわない
	if (mutekiTimer.GetRun())return;
	//無敵時間さん!?を開始
	mutekiTimer.Start();

	//ダメージ受けたらノックバックしちゃおう
	hp -= damage;

	attackTarget = target_;

	knockbackVec = dir;

	//ノックバックのスピード加算
	knockbackSpeed = knockbackRange;

	//角度計算(-90度~90度)
	knockRadianStart.x = Util::GetRand(
		EnemyManager::GetInstance()->knockRandXS,
		EnemyManager::GetInstance()->knockRandXE);
	//縦のけぞり計算(45~90度)
	knockRadianStart.z = Util::GetRand(
		EnemyManager::GetInstance()->knockRandZS,
		EnemyManager::GetInstance()->knockRandZE);

	//ヒットモーションタイマーを開始
	knockbackTimer.Start();

	//ヒットパーティクル出す
	ParticleManager::GetInstance()->AddSimple(
		obj.mTransform.position, obj.mTransform.scale,
		10, 0.3f, obj.mTuneMaterial.mColor, "", 0.5f, 1.2f,
		dir.GetNormalize() - Vector3::ONE * 0.3f, dir.GetNormalize() - Vector3::ONE * 0.3f,
		0.03f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.1f);

	//ヒットエフェクト出す
	ParticleManager::GetInstance()->AddSimple(
		obj.mTransform.position + Vector3::UP * obj.mTransform.scale.y,
		obj.mTransform.scale * 0.f, 1, 0.3f,
		obj.mTuneMaterial.mColor, TextureManager::Load("./Resources/hit_circle.png"),
		0.f, 0.f, Vector3::ZERO, Vector3::ZERO,
		0.f, Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.1f, 3.f, false, true);

	//かかりカウント加算
	waxSolidCount++;
	waxShakeOffTimer = 0;
}

void Enemy::SetDeath()
{
	isAlive = false;
}