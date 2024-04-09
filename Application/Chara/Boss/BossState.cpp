#include "BossState.h"
#include "Boss.h"

BossNormal::BossNormal()
{
	priority = 0;
	floatingTimer = 2.f;
}

void BossNormal::Update(Boss* boss)
{
	boss->SetStateStr("Normal");

	//プレイヤーに向かって移動するAI
	Vector3 aVec = boss->GetTarget()->mTransform.position - boss->obj.mTransform.position;
	aVec.Normalize();
	aVec.y = 0;

	//ターゲットの方向を向いてくれる
	Quaternion aLookat = Quaternion::LookAt(aVec);

	//euler軸へ変換
	boss->obj.mTransform.rotation = aLookat.ToEuler();

	floatingTimer.RoopReverse();

	for (size_t i = 0; i < boss->parts.size(); i++)
	{
		boss->parts[i].mTransform.position.y = 
			boss->handOriPos[i].y + Easing::InQuad(floatingTimer.GetTimeRate()) * 0.03f;
	}
}

BossPunch::BossPunch()
{
}

void BossPunch::Update(Boss* boss)
{
	boss;
}