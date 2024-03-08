#include "Enemy.h"
#include "Camera.h"

Enemy::Enemy(ModelObj* target_) : GameObject(),
	moveSpeed(0.1f),slowMag(0.8f), isGraund(true), isSlow(false), hp(0), maxHP(10)
{
	obj = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));
	target = target_;
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

	if (isSlow == false)
	{
		slowMag = 0.f;
	}

	//減速率は大きいほどスピード下がるから1.0から引くようにしてる
	obj.mTransform.position += pVec * moveSpeed * (1.f - slowMag);

	UpdateCollider();

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Enemy::Draw()
{
	if (isAlive)
	{
		obj.Draw();
	}
}

void Enemy::Tracking()
{
	//プレイヤーに向かって移動するAI
	Vector3 pVec = target->mTransform.position - obj.mTransform.position;
	pVec.Normalize();
	pVec.y = 0;

	//減速率は大きいほどスピード下がるから1.0から引くようにしてる
	obj.mTransform.position += pVec * moveSpeed * (1.f - slowMag);
}

