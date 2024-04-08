#include "Enemy.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "ParticleManager.h"
#include "Parameter.h"
#include "Quaternion.h"
#include "RImGui.h"
#include "Renderer.h"
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

	predictionLine = ModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube"));
	attackDrawerObj = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));
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
	if (!GetIsSolid()) {
		//各ステート時の固有処理
		state->Update(this);	//移動速度に関係するので移動の更新より前に置く


		//前のステートと異なれば
		if (changingState) {
			//ステートを変化させる
			std::swap(state, nextState);
			changingState = false;
			nextState = nullptr;
		}

		//ステートに入る前に、予測線は消しておきたいのでスケールを0に
		predictionLine.mTransform.scale = { 0.f,0.f,0.f };
		//予測線を使う場合はこの中で値を変える
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

	//無敵時間さん!?の更新
	mutekiTimer.Update();

	//プレイヤーに向かって移動するAI
	//普段はプレイヤーにではなく、ランダムだったり特定方向へ進み続けて、
	//ぶつかって初めてターゲットに入れるようにしたい
	Vector3 pVec = target->mTransform.position - obj.mTransform.position;
	pVec.Normalize();
	pVec.y = 0;


	//ノックバックとかのけぞりとか
	//攻撃中はそっちの回転を優先
	if (GetAttackState() == "NowAttack") {
		//汚いけど、ステート内で回転しちゃってるのでここでは何も書かない
	}
	else {
		KnockBack();
		KnockRota();

		if (!GetIsSolid()) {
			//通常時の回転(固まってるときは回転しないように)
			Rotation(pVec);
		}
	}

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
	UpdateAttackCollider();

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	obj.mPaintDataBuff->dissolveVal = waxSolidCount >= requireWaxSolidCount ? 1.0f : 0.3f / (requireWaxSolidCount - 1) * waxSolidCount;
	obj.mPaintDataBuff->color = Color(0.8f, 0.6f, 0.35f, 1.0f);
	obj.mPaintDataBuff->slide += TimeManager::deltaTime;
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);

	ui.Update(this);

	predictionLine.mTransform.UpdateMatrix();
	predictionLine.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Enemy::Draw()
{
	if (isAlive)
	{
		obj.Draw();
		ui.Draw();
		predictionLine.Draw();

		DrawCollider();
		DrawAttackCollider();
	}
}

void Enemy::KnockBack()
{
	//ノックバック時間の更新
	knockbackTimer.Update();

	//ノックバックを加算
	moveVec += knockbackVec * knockbackSpeed;

	//ノックバックを減少
	knockbackSpeed = Easing::OutQuad(knockbackRange, 0, knockbackTimer.GetTimeRate());

	//ノックバックが終わったら
	if (knockbackTimer.GetEnd()) {
		//ターゲットを解除
		attackTarget = nullptr;
	}
}

void Enemy::KnockRota()
{
	//攻撃されたら
	if (attackTarget) 
	{
		//プレイヤーに向かって移動するAI
		Vector3 aVec = attackTarget->mTransform.position - obj.mTransform.position;
		aVec.Normalize();
		aVec.y = 0;

		//ターゲットの方向を向いてくれる
		Quaternion aLookat = Quaternion::LookAt(aVec);

		//喰らい時のモーション遷移
		knockRadianX = Easing::InQuad(knockRadianStart.x, 0, knockbackTimer.GetTimeRate());
		knockRadianZ = Easing::InQuad(knockRadianStart.z, 0, knockbackTimer.GetTimeRate());

		Quaternion knockQuaterX = Quaternion::AngleAxis({ 1,0,0 }, knockRadianX);
		Quaternion knockQuaterZ = Quaternion::AngleAxis({ 0,0,1 }, knockRadianZ);

		//計算した向きをかける
		aLookat = aLookat * knockQuaterX * knockQuaterZ;

		//euler軸へ変換
		obj.mTransform.rotation = aLookat.ToEuler();
	}
}

void Enemy::Rotation(const Vector3& pVec)
{
	//普段はターゲットの方向を向く
	Quaternion pLookat = Quaternion::LookAt(pVec);
	//euler軸へ変換
	obj.mTransform.rotation = pLookat.ToEuler();
}

void Enemy::SetTarget(ModelObj* target_)
{
	target = target_;
}

ModelObj* Enemy::GetTarget()
{
	return target;
}

bool Enemy::GetIsSolid()
{
	return waxSolidCount >= requireWaxSolidCount;
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

void Enemy::MoveVecPlus(const Vector3& plusVec)
{
	moveVec += plusVec;
}

void Enemy::UpdateAttackCollider()
{
	attackHitCollider.pos = GetPos();

	attackDrawerObj.mTuneMaterial.mColor = { 1,1,0,1 };
	attackDrawerObj.mTransform.position = attackHitCollider.pos;
	float r = attackHitCollider.r;
	attackDrawerObj.mTransform.scale = { r,r,r };

	attackDrawerObj.mTransform.UpdateMatrix();
	attackDrawerObj.TransferBuffer(Camera::sNowCamera->mViewProjection);

}

void Enemy::DrawAttackCollider()
{
	//描画しないならスキップ
	if (!isDrawCollider)return;

	//パイプラインをワイヤーフレームに
	PipelineStateDesc pipedesc = RDirectX::GetDefPipeline().mDesc;
	pipedesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

	GraphicsPipeline pipe = GraphicsPipeline::GetOrCreate("WireObject", pipedesc);
	for (RenderOrder& order : attackDrawerObj.GetRenderOrder()) {
		order.pipelineState = pipe.mPtr.Get();
		Renderer::DrawCall("Opaque", order);
	}
}
