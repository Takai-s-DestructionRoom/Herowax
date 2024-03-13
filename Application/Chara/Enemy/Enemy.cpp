#include "Enemy.h"
#include "Camera.h"

Enemy::Enemy(ModelObj* target_) : GameObject(),
	moveSpeed(0.1f),slowMag(0.8f),
	isGraund(true), hp(0), maxHP(10.f),
	isEscape(false),escapePower(0.f),escapeCoolTimer(1.f),
	isAttack(false), atkPower(0.f), atkCoolTimer(1.f)
{
	state = std::make_unique<EnemyNormal>();
	obj = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));
	target = target_;
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
	obj.mTransform.position += pVec * moveSpeed * 
		(1.f - slowMag) * (1.f - slowCoatingMag);
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

void Enemy::SetDeath()
{
	isAlive = false;
}
