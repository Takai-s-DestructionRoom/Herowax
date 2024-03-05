#include "Enemy.h"
#include "Camera.h"

Enemy::Enemy(ModelObj* target_)
{
	moveSpeed = 0.5f;
	obj = ModelObj(Model::Load("./Resources/Model/Cube.obj", "Sphere", true));
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

	obj.mTransform.position += pVec * moveSpeed;

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Enemy::Draw()
{
	obj.Draw();
}

