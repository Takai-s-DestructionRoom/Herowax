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
	//吸収状態になったら反転を解除
	wax->isReverse = false;

	oldTimeRate = wax->collectTimer.GetTimeRate();
	wax->collectTimer.Update();
	if (isStart) {
		wax->collectTimer.Start();
		isStart = false;
		startPos = wax->obj.mTransform.position;
	}
	wax->SetStateStr(WaxCollect::GetStateStr());

	Vector3 now = OutQuadVec3(startPos, wax->collectPos, wax->collectTimer.GetTimeRate());
	Vector3 old = OutQuadVec3(startPos, wax->collectPos, oldTimeRate);
	//現在フレームの移動量の分だけをmoveVecに足す
	wax->moveVec += now - old;

	//到達したら殺す
	if (wax->collectTimer.GetEnd())
	{
		wax->DeadParticle();
		wax->isAlive = false;
		accel = 0.f;

		WaxManager::GetInstance()->isCollected = true;
	}
}

std::string WaxCollect::GetStateStr()
{
	return "WaxCollect";
}