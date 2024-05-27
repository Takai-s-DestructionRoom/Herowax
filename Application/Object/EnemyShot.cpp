#include "EnemyShot.h"
#include "Quaternion.h"

void EnemyShot::LoadResource()
{ 
	//モデル読み込み
	Model::Load("./Resources/Model/hexagonBolt/hexagonBolt.obj", "hexagonBolt");
	Model::Load("./Resources/Model/gear/gear.obj", "gear");
	Model::Load("./Resources/Model/plusScrew/plusScrew.obj", "plusScrew");
}

void EnemyShot::SetParam(const Vector3& position,const Vector3& moveVec_)
{
	obj.mTransform.position = position;
	moveVec = moveVec_;
}

void EnemyShot::Init()
{
	int32_t rand = Util::GetRand(0, 2);
	if (rand == 0) {
		obj = PaintableModelObj("hexagonBolt");
	}
	if (rand == 1) {
		obj = PaintableModelObj("gear");
	}
	if (rand == 2) {
		obj = PaintableModelObj("plusScrew");
	}

	obj.mTransform.scale = { 3,3,3 };

	lifeTimer.Start();
}

void EnemyShot::Update()
{
	lifeTimer.Update();
	if (lifeTimer.GetEnd()) {
		isAlive = false;
	}

	rotVec.y += rotSpeed;

	//指定した場所までくるくるーって飛んでいく
	//座標加算
	obj.mTransform.position += moveVec * moveSpeed;

	//回転の適用
	//指定した方向を向く
	obj.mTransform.rotation = Quaternion::LookAt(moveVec).ToEuler() + rotVec;

	BrightTransferBuffer(Camera::sNowCamera->mViewProjection);
	UpdateCollider();

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
}

void EnemyShot::Draw()
{
	if (isAlive) {
		BrightDraw();
	}
}

float EnemyShot::GetDamage()
{
	return damage;
}

void EnemyShot::SetDamage(float damage_)
{
	damage = damage_;
}

void EnemyShot::SetMoveSpeed(float moveSpeed_)
{
	moveSpeed = moveSpeed_;
}

void EnemyShot::SetLifeTime(float lifeTime_)
{
	lifeTimer.maxTime_ = lifeTime_;
}

void EnemyShot::Reversal()
{
	//反転済みなら通さない
	if (isReversal)return;

	isReversal = true;
	moveVec = -moveVec;
}
