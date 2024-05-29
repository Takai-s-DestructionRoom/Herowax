#include "WaxState.h"
#include "Wax.h"
#include "Easing.h"
#include "Temperature.h"
#include "WaxManager.h"
#include "ParticleManager.h"

void WaxNormal::Update(Wax* wax)
{
	wax->SetStateStr(WaxNormal::GetStateStr());
}

std::string WaxNormal::GetStateStr()
{
	return "Normal";
}

WaxCollect::WaxCollect()
{
	isStart = true;
}

void WaxCollect::Update(Wax* wax)
{
	//もし回収対象がぬるぽになってたら解除
	if (WaxManager::GetInstance()->collectTarget == nullptr) {
		wax->ChangeState<WaxNormal>();
		return;
	}

	//吸収状態になったら反転を解除
	wax->isReverse = false;

	if (isStart) {
		isStart = false;
		startPos = wax->obj.mTransform.position;
	}

	timer.Roop();
	if (timer.GetRoopEnd()) {
		accel += 0.1f;
	}

	wax->SetStateStr(WaxCollect::GetStateStr());

	Vector3 tVec = WaxManager::GetInstance()->collectTarget->obj.mTransform.position - startPos;
	tVec.Normalize();
	tVec.y = 0;
	
	//現在フレームの移動量の分だけをmoveVecに足す
	wax->moveVec += tVec * collectSpeed * accel;

	//到達したら殺す
	if (ColPrimitive3D::CheckSphereToSphere(wax->collider, WaxManager::GetInstance()->collectTarget->collider) &&
		wax->isAlive)
	{ 
		wax->DeadParticle();
		wax->isAlive = false;
		accel = 0.f;

		WaxManager::GetInstance()->collectWaxNum++;
	}
}

std::string WaxCollect::GetStateStr()
{
	return "WaxCollect";
}