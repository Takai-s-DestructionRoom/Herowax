#include "Enemy.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "ParticleManager.h"
#include "Parameter.h"
#include "Level.h"
#include "Quaternion.h"
#include "RImGui.h"
#include "Renderer.h"
#include <TimeManager.h>
#include "InstantDrawer.h"
#include "WaxManager.h"


Enemy::Enemy(ModelObj* target_) : GameObject(),
moveSpeed(0.1f), slowMag(0.8f),
isGraund(true), hp(0), maxHP(10.f),
isEscape(false), escapePower(0.f), escapeCoolTimer(1.f),
isAttack(false), atkPower(0.f), atkCoolTimer(1.f),
gravity(0.2f)
{
	state = std::make_unique<EnemyNormal>();
	attackState = std::make_unique<EnemyNormalState>();
	nextState = nullptr;
	nextAttackState = nullptr;
	obj = PaintableModelObj(Model::Load("./Resources/Model/firewisp/firewisp.obj", "firewisp", true));
	obj.mPaintDissolveMapTex = TextureManager::Load("./Resources/DissolveMap.png", "DissolveMapTex");
	target = target_;

	std::map<std::string, std::string> extract = Parameter::Extract("Enemy");
	knockbackTimer.maxTime_ = Parameter::GetParam(extract, "ノックバックにかかる時間", 0.5f);
	knockbackRange = Parameter::GetParam(extract, "ノックバックする距離", 0.5f);

	mutekiTimer.maxTime_ = Parameter::GetParam(extract, "無敵時間さん", 0.1f);

	oriScale = { 3,3,3 };
	obj.mTransform.scale = oriScale;
	baseScale = oriScale;

	predictionLine = ModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube"));
	//影をなくす
	predictionLine.mTuneMaterial.mAmbient = Vector3(1, 1, 1) * 100.f;
	predictionLine.mTuneMaterial.mDiffuse = Vector3::ZERO;
	predictionLine.mTuneMaterial.mSpecular = Vector3::ZERO;

	attackDrawerObj = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));

	TextureManager::Load("./Resources/warning.png", "warning");

	//shield.obj.SetParent(&obj);
}

Enemy::~Enemy()
{
	////50%の確率でドロップ
	//int32_t rand = Util::GetRand(0, 1);
	//if (rand == 1) {
	//	CollectPartManager::GetInstance()->Craete(obj.mTransform.position);
	//}
}

void Enemy::Init()
{
	hp = maxHP;

	basis = obj.mTransform.position;
	behaviorOrigen = obj.mTransform.position;
}

void Enemy::Reset()
{
	moveVec.x = 0;
	moveVec.z = 0;

	rotVec = { 0,0,0 };

	//ステートに入る前に、前フレームに足したシェイクを引いて元に戻す
	obj.mTransform.position -= shack;
	//シェイクを元に戻す
	shack = { 0,0,0 };

	SetForceRot(false);
}

void Enemy::Update()
{
	BaseUpdate();
}

void Enemy::BaseUpdate()
{
	spawnTimer.Update();
	warningTimer.Update();

	if (warningTimer.GetEnd()) {
		if (!spawnTimer.GetStarted())spawnTimer.Start();

		warningRoop.RoopReverse();
		warningColor.a = Easing::OutQuad(0.f, 1.f, warningRoop.GetTimeRate());
	}

	if (spawnTimer.GetRun())
	{
		ParticleManager::GetInstance()->AddHoming(
			obj.mTransform.position + Vector3::UP * obj.mTransform.scale.y, "enemy_pop_homing");
		//パーティクル出現
		static int32_t count;
		count++;
		if (count > 5)
		{
			ParticleManager::GetInstance()->AddSimple(
				obj.mTransform.position + Vector3::UP * obj.mTransform.scale.y, "smoke");
			count = 0;
		}
	}

	if (spawnTimer.GetNowEnd()) {
		//パーティクル出現
		ParticleManager::GetInstance()->AddSimple(
			obj.mTransform.position + Vector3::UP * obj.mTransform.scale.y, "smoke_red");
	}

	//こっから下は出現してからなので
	//出現タイマーが終わるまで通さない
	if (!GetIsSpawn())return;

	Reset();

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

	//各ステート時の固有処理
	state->Update(this);	//移動速度に関係するので移動の更新より前に置く

	//前のステートと異なれば
	if (changingState) {
		//ステートを変化させる
		std::swap(state, nextState);
		changingState = false;
		nextState = nullptr;
	}

	SlowUpdate();

	//固まっていなければする処理
	if (!GetIsSolid()) {
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

	if (!GetIsSolid()) {
		//攻撃準備に入ったらプレイヤーへ向けた移動をしない
		//(ステート内に書いてもいいけどどこにあるかわからなくなりそうなのでここで)
		if (GetAttackState() == EnemyNormalState::GetStateStr())
		{
			//通常移動をオーダーをもとにやる
			Vector3 behaviorDir = loadBehaviorData.GetBehavior(behaviorOrigen, obj.mTransform.position).GetNormalize();
			moveVec += behaviorDir * moveSpeed;
		}
	}

	hp = Util::Clamp(hp, 0.f, maxHP);

	if (hp <= 0 && isCollect == false) {
		//hpが0になったら、自身の状態を固まり状態へ遷移
		//現在毎フレーム通常状態に戻す処理を行っているので無意味
		ChangeState<EnemyAllStop>();
	}

	//無敵時間さん!?の更新
	mutekiTimer.Update();

	///-------------移動、回転の加算--------------///

	//ノックバック中でないなら重力をかける
	if (!knockbackTimer.GetRun()) {
		//重力をかける
		moveVec.y -= gravity;
	}

	//ノックバックも攻撃準備に入ったら無効化する
	if (GetAttackState() != "NowAttack" &&
		GetAttackState() != "PreAttack")
	{
		KnockBack();
		KnockRota();
	}

	//いろいろ足した後減速
	//減速率は大きいほどスピード下がるから1.0から引くようにしてる
	if (isCollect == false)
	{
		moveVec *= (1.f - slowMag) * (1.f - slowCoatingMag);
	}

	//シェイクの値
	moveVec += shack;

	//座標加算
	obj.mTransform.position += moveVec;

	//地面座標を下回るなら戻す
	if (obj.mTransform.position.y <= Level::Get()->ground->GetTransform().position.y) {
		obj.mTransform.position.y = Level::Get()->ground->GetTransform().position.y;
		moveVec.y = 0;
	}

	//もし何も回転の加算がない場合、
	if (!forceRot) {
		//固まっていなければ通常時の回転をする
		if (!GetIsSolid() && !knockbackTimer.GetRun()) {
			Vector3 tVec = loadBehaviorData.GetMoveDir();
			tVec.Normalize();
			tVec.y = 0;

			Rotation(tVec);
		}
	}
	//回転の適用
	obj.mTransform.rotation = rotVec;
	scalingTimer.Update();
	float solid = Easing::GoAndBackInQuad(0.0f, 3.0f, scalingTimer.GetTimeRate());
	obj.mTransform.scale = baseScale;
	obj.mTransform.scale += {solid, solid, solid};

	if (GetIsSolid()) {
		obj.mTuneMaterial.mColor = changeColor;
	}

	//色の加算
	whiteTimer.Update();
	brightColor.r = Easing::OutQuad(1, 0, whiteTimer.GetTimeRate());
	brightColor.g = Easing::OutQuad(1, 0, whiteTimer.GetTimeRate());
	brightColor.b = Easing::OutQuad(1, 0, whiteTimer.GetTimeRate());
	brightColor.a = Easing::OutQuad(1, 0, whiteTimer.GetTimeRate());
	if (!whiteTimer.GetStarted())brightColor = { 0,0,0,0 };

	//WaxVisualUpdate();
}

void Enemy::TransfarBuffer()
{
	UpdateCollider();
	UpdateAttackCollider();

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	obj.mPaintDataBuff->dissolveVal = waxSolidCount >= requireWaxSolidCount ? 1.0f : 0.3f / (requireWaxSolidCount - 1) * waxSolidCount;
	//obj.mPaintDataBuff->dissolveVal = 1.0f;
	obj.mPaintDataBuff->color = WaxManager::GetInstance()->slimeWax.waxColor;
	obj.mPaintDataBuff->slide += TimeManager::deltaTime;
	BrightTransferBuffer(Camera::sNowCamera->mViewProjection);

	ui.Update(this);

	predictionLine.mTransform.UpdateMatrix();
	predictionLine.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Enemy::Draw()
{
	////警告を表示
	//if (spawnTimer.GetRun()) {
	//	Vector3 pos = obj.mTransform.position;
	//	pos.y += obj.mTransform.scale.y;
	//	InstantDrawer::DrawGraph3D(pos, 10.f, 10.f, "warning", warningColor);
	//}

	if (!spawnTimer.GetEnd()) return;

	if (isAlive)
	{
		BrightDraw();

		ui.Draw();

		if (!GetIsSolid()) {
			predictionLine.Draw();
		}

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
		RotVecPlus(aLookat.ToEuler());
	}
}

void Enemy::Rotation(const Vector3& pVec)
{
	//普段はターゲットの方向を向く
	Quaternion pLookat = Quaternion::LookAt(pVec);
	//euler軸へ変換
	RotVecPlus(pLookat.ToEuler());
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

Vector3 Enemy::GetOriginPos()
{
	if (loadBehaviorData.points.size() > 0)
	{
		return basis + loadBehaviorData.points[0];
	}
	return basis;
}

bool Enemy::GetIsSpawn()
{
	return spawnTimer.GetEnd();
}

void Enemy::BehaviorReset()
{
	//タイマーなど初期化
	loadBehaviorData.Init();
	//基準座標をリセット
	BehaviorOrigenReset();
}

void Enemy::CreateWaxVisual(Vector3 spawnPos)
{
	waxVisual.emplace_back();
	//差分だけ保持
	waxVisual.back() = std::make_unique<WaxVisual>();
	waxVisual.back()->obj.SetParent(&obj);
	waxVisual.back()->Init();

	if (spawnPos.LengthSq() != 0) {
		waxVisual.back()->obj.mTransform.position = spawnPos - obj.mTransform.position;
	}
}


void Enemy::WaxVisualUpdate()
{
	for (auto itr = waxVisual.begin(); itr != waxVisual.end();)
	{
		//死んでたら殺す
		if (!(itr->get())->isAlive) {
			itr = waxVisual.erase(itr);
		}
		else {
			itr++;
		}
	}

	/*for (auto& wax1 : waxVisual)
	{
		for (auto& wax2 : waxVisual)
		{
			if (wax1 == wax2)continue;

			重なりチェック
			if (ColPrimitive3D::CheckSphereToSphere(wax1->collider, wax2->collider))
			{
				wax1->power++;
				wax2->power++;
			}
		}
	}*/

	for (auto& wax : waxVisual)
	{
		bool check = false;
		wax->Update();

		//当たり判定
		while (ColPrimitive3D::CheckSphereToSphere(wax->collider, collider))
		{
			Vector3 repulsionVec = wax->collider.pos - collider.pos;
			repulsionVec.Normalize();
			repulsionVec.y = 0;

			wax->collider.pos += repulsionVec;

			//もしここが0になった場合無限ループするので抜ける
			if (repulsionVec.LengthSq() == 0) {
				break;
			}
			check = true;
			wax->obj.SetParent(&obj);
		}

		wax->TransferBuffer();
	}
}

void Enemy::SlowUpdate()
{
	if (GetState() == "Slow" && GetIsSolid() == false)
	{
		Quaternion aLookat =
			Quaternion::LookAt(moveVec);

		//固有の足とられモーション
		motionTimer.Update();
		if (motionTimer.GetStarted() == false)
		{
			motionTimer.Start();
		}
		else if (motionTimer.GetEnd())
		{
			motionFrag = motionFrag ^ motionTimer.GetEnd();
			motionTimer.Reset();
		}


		if (motionFrag)
		{
			radianX = Easing::OutQuad(Util::AngleToRadian(-30.f), Util::AngleToRadian(50.f), motionTimer.GetTimeRate());
			radianZ = Easing::InQuad(Util::AngleToRadian(30.f), Util::AngleToRadian(-30.f), motionTimer.GetTimeRate());
		}
		else
		{
			radianX = Easing::OutQuad(Util::AngleToRadian(50.f), Util::AngleToRadian(-30.f), motionTimer.GetTimeRate());
			radianZ = Easing::InQuad(Util::AngleToRadian(-30.f), Util::AngleToRadian(30.f), motionTimer.GetTimeRate());
		}

		Quaternion rotX = Quaternion::AngleAxis({ 1,0,0 }, radianX);
		Quaternion rotZ = Quaternion::AngleAxis({ 0,0,1 }, radianZ);

		aLookat = rotX * rotZ * aLookat;

		RotVecPlus(aLookat.ToEuler());
	}
}

void Enemy::StartToMoving()
{
	warningTimer.Start();
}

void Enemy::DealDamage(uint32_t damage, const Vector3& dir, ModelObj* target_)
{
	//無敵時間さん!?中かHP0なら攻撃を喰らわない
	if (mutekiTimer.GetRun() || GetIsSolid())return;
	//無敵時間さん!?を開始
	mutekiTimer.Start();

	//ダメージ受けたらノックバックしちゃおう
	hp -= damage;

	attackTarget = target_;

	knockbackVec = dir;

	//whiteTimer.maxTime_ = mutekiTimer.maxTime_ / 4;
	whiteTimer.Start();

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

	//ヒットパーティクル出す(攻撃方向に依存するのでエディタのは使えない)
	ParticleManager::GetInstance()->AddSimple(
		obj.mTransform.position, obj.mTransform.scale,
		10, 0.3f, obj.mTuneMaterial.mColor, "", 0.5f, 1.2f,
		dir.GetNormalize() - Vector3(1, 1, 1) * 0.3f, dir.GetNormalize() - Vector3(1, 1, 1) * 0.3f,
		0.03f, -Vector3(1, 1, 1) * 0.1f, Vector3(1, 1, 1) * 0.1f, 0.1f);

	//ヒットエフェクト出す
	Vector3 toPlayerVec =
		target->mTransform.position - obj.mTransform.position;	//プレイヤーへのベクトル

	ParticleManager::GetInstance()->AddSimple(
		obj.mTransform.position +
		Vector3::UP * obj.mTransform.scale.y +
		toPlayerVec.Normalize() * obj.mTransform.scale.z,
		"enemy_hit");
	ParticleManager::GetInstance()->AddSimple(
		obj.mTransform.position +
		Vector3::UP * obj.mTransform.scale.y +
		toPlayerVec.Normalize() * obj.mTransform.scale.z,
		"enemy_hit_inside");

	//かかりカウント加算
	waxSolidCount++;
	waxShakeOffTimer = 0;

	//固まった瞬間
	if (GetIsSolid())
	{
		ParticleManager::GetInstance()->AddHoming(obj.mTransform.position, "enemy_solid_homing");
		ParticleManager::GetInstance()->AddSimple(
			obj.mTransform.position + Vector3::UP * obj.mTransform.scale.y, "enemy_solid");

		//拡縮タイマーを開始
		scalingTimer.Start();
	}
}

void Enemy::SetDeath()
{
	isAlive = false;

	//死んだときパーティクル出す
	ParticleManager::GetInstance()->AddSimple(
		obj.mTransform.position, "enemy_dead");
}

void Enemy::MoveVecPlus(const Vector3& plusVec)
{
	moveVec += plusVec;
}

void Enemy::RotVecPlus(const Vector3& plusVec)
{
	rotVec += plusVec;
	SetForceRot(true);
}

void Enemy::SetRotVec(const Vector3& rotationVec)
{
	rotVec = rotationVec;
	SetForceRot(true);
}

void Enemy::SetBehaviorOrder(const std::string& order)
{
	loadFileName = order;
	loadBehaviorData = EnemyBehaviorEditor::Load(loadFileName);
	loadBehaviorData.Init();
}

void Enemy::SetEnemyOrder(const std::string& order)
{
	EnemyData data = EnemyBehaviorEditor::LoadEnemyData(order);

	spawnTimer = data.spawnTime;
	warningTimer = data.warningTime;

	warningRoop.maxTime_ = spawnTimer.maxTime_ / 4;
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
