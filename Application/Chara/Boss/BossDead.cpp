#include "BossDead.h"
#include "Boss.h"
#include "ParticleManager.h"

BossDead::BossDead()
{
	priority = 0;
}

void BossDead::Update(Boss* boss)
{
	boss->SetStateStr("Dead");	//ステートがわかるように設定しとく
	explosionTimer.Update();

	if (isStart)
	{
		explosionTimer.Start();

		isStart = false;
	}

	//
	Vector3 vec = Vector3(0, 0, -1);
	vec.Normalize();
	vec.y = 0;

	//ターゲットの方向を向いてくれる
	Quaternion lookat = Quaternion::LookAt(vec);

	//euler軸へ変換
	boss->obj.mTransform.rotation = lookat.ToEuler();

	////手ないはずだからいらない
	//for (size_t i = 0; i < boss->parts.size(); i++)
	//{
	//	//基準座標に回転をかけて親子っぽくしてる
	//	boss->parts[i].obj.mTransform.position = boss->parts[i].oriPos * Matrix4::RotationY(lookat.ToEuler().y);
	//	boss->parts[i].obj.mTransform.rotation = lookat.ToEuler();
	//}

	/*if (boss->isAlive == false)
	{
		ParticleManager::GetInstance()->AddSimple(boss->GetCenterPos(), "enemy_dead");
	}*/
}