#include "BossNormal.h"
#include "Boss.h"
#include "Matrix4.h"

BossNormal::BossNormal()
{
	priority = 0;
	floatingTimer = 2.f;
}

void BossNormal::Update(Boss* boss)
{
	boss->SetStateStr("Normal");	//ステートがわかるように設定しとく

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
		//基準座標に回転をかけて親子っぽくしてる
		boss->parts[i].obj.mTransform.position = boss->parts[i].oriPos * Matrix4::RotationY(aLookat.ToEuler().y);
		boss->parts[i].obj.mTransform.rotation = aLookat.ToEuler();

		//ふよふよ
		boss->parts[i].obj.mTransform.position.y =
			boss->parts[i].oriPos.y + Easing::InQuad(floatingTimer.GetTimeRate()) * 3.f;
	}
}