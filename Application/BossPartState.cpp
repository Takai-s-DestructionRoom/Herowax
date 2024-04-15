#include "BossPartState.h"
#include "BossPart.h"

void BossPartNormal::Update(Parts* parts)
{
	//何もしない
	parts;
}

BossPartCollect::BossPartCollect()
{
	isStart = true;
	lifeTimer.Start();
}

void BossPartCollect::Update(Parts* parts)
{
	if (isStart) {
		startPos = parts->GetPos();
		startScale = parts->GetScale();
		startRota = parts->obj.mTransform.rotation;
		endRota = startRota + Vector3(0, Util::AngleToRadian(360) * 2, 0);//2回転
		isStart = false;
	}

	//吸収されて死ぬ
	lifeTimer.Update();

	parts->SetPos(InQuadVec3(startPos,parts->collectPos,lifeTimer.GetTimeRate()));
	parts->SetScale(InQuadVec3(startScale, {0,0,0}, lifeTimer.GetTimeRate()));
	parts->SetRota(InQuadVec3(startRota, endRota, lifeTimer.GetTimeRate()));

	if (lifeTimer.GetEnd()) {
		//切り替え
		parts->ChangeState<BossPartNormal>();

		//死亡
		parts->isAlive = false;
	}
}