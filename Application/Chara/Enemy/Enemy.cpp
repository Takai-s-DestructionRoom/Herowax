#include "Enemy.h"
#include "Camera.h"
#include "EnemyManager.h"

Enemy::Enemy(ModelObj* target_) : GameObject(),
	moveSpeed(0.1f),slowMag(0.8f),
	isGraund(true), hp(0), maxHP(10.f),
	isEscape(false),escapePower(0.f),escapeCoolTimer(1.f),
	isAttack(false), atkPower(0.f), atkCoolTimer(1.f),
	gravity(0.2f), groundPos(0)
{
	state = std::make_unique<EnemyNormal>();
	obj = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));
	target = target_;
	knockbackTimer.maxTime_ = 0.5f;
}

Enemy::~Enemy()
{
	
}

void Enemy::Init()
{
	hp = maxHP;
}

void Enemy::Update()
{
	moveVec.x = 0;
	moveVec.z = 0;

	//プレイヤーに向かって移動するAI
	Vector3 pVec = target->mTransform.position - obj.mTransform.position;
	pVec.Normalize();
	pVec.y = 0;

	//各ステート時の固有処理
	state->Update(this);	//移動速度に関係するので移動の更新より前に置く

	if (hp <= 0) {
		SetDeath();
	}

	//減速率は大きいほどスピード下がるから1.0から引くようにしてる
	moveVec += pVec * moveSpeed *
		(1.f - slowMag) * (1.f - slowCoatingMag);
	
	//ノックバックさせる
	knockbackTimer.Update();

	//ノックバック中でないなら重力をかける
	if (!knockbackTimer.GetRun()) {
		//重力をかける
		moveVec.y -= gravity;
	}

	//座標加算
	obj.mTransform.position += moveVec;

	//ノックバック中ならノックバックする
	//あとから上書きしてんのマジでカス。何とかする
	if (knockbackTimer.GetRun()) {
		obj.mTransform.position.x = Easing::OutQuad(knockbackVecS.x, knockbackVecE.x, knockbackTimer.GetTimeRate());
		obj.mTransform.position.y = Easing::OutQuad(knockbackVecS.y, knockbackVecE.y, knockbackTimer.GetTimeRate());
		obj.mTransform.position.z = Easing::OutQuad(knockbackVecS.z, knockbackVecE.z, knockbackTimer.GetTimeRate());
	}

	//地面座標を下回るなら戻す
	if (obj.mTransform.position.y <= groundPos) {
		obj.mTransform.position.y = groundPos;
		moveVec.y = 0;
	}

	UpdateCollider();

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);

	ui.Update(this);
}

void Enemy::Draw()
{
	if (isAlive)
	{
		obj.Draw();
		ui.Draw();
	}
}

void Enemy::Tracking()
{
	//プレイヤーに向かって移動するAI
	Vector3 pVec = target->mTransform.position - obj.mTransform.position;
	pVec.Normalize();
	pVec.y = 0;

	//減速率は大きいほどスピード下がるから1.0から引くようにしてる
	obj.mTransform.position += pVec * moveSpeed *
		(1.f - slowMag) * (1.f - slowCoatingMag);
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

void Enemy::DealDamage(uint32_t damage, const Vector3& dir)
{
	//ダメージ受けたらノックバックしちゃおう
	hp -= damage;

	knockbackVecS = obj.mTransform.position;
	knockbackVecE = obj.mTransform.position + dir.GetNormalize() * EnemyManager::GetInstance()->GetKnockBack();
	knockbackTimer.Start();
}

void Enemy::SetDeath()
{
	isAlive = false;
}
