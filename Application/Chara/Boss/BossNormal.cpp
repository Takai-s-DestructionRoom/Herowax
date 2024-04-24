#include "BossNormal.h"
#include "Boss.h"
#include "Matrix4.h"

BossNormal::BossNormal()
{
	priority = 0;
	floatingTimer = 2.f;
	interpolationTimer = 0.5f;
	interpolationTimer.Reset();
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

	// 補間の処理 //
	//どっちかの手が固まってるなら補間はスルー
	if ((boss->parts[0].GetIsSolid() && boss->parts[0].GetIsAlive()) ||
		(boss->parts[1].GetIsSolid() && boss->parts[1].GetIsAlive()))
	{
	}
	//どちらの手も固まってないなら補間
	else
	{
		interpolationTimer.Update();
	}

	if (interpolationTimer.GetStarted() == false)
	{
		interpolationTimer.Start();

		startObjRot = boss->obj.mTransform.rotation;

		//今の座標と回転情報取得
		for (size_t i = 0; i < boss->parts.size(); i++)
		{
			startPos[i] = boss->parts[i].obj.mTransform.position;
			startRot[i] = boss->parts[i].obj.mTransform.rotation;
		}
	}

	if (interpolationTimer.GetRun())
	{
		//今の座標から基準座標まで補間
		boss->obj.mTransform.rotation =
			InBackVec3(startObjRot, aLookat.ToEuler(), interpolationTimer.GetTimeRate());

		for (size_t i = 0; i < boss->parts.size(); i++)
		{
			Vector3 oriPos = boss->parts[i].oriPos * Matrix4::RotationY(aLookat.ToEuler().y);

			boss->parts[i].obj.mTransform.position =
				InBackVec3(startPos[i], oriPos, interpolationTimer.GetTimeRate());
			boss->parts[i].obj.mTransform.rotation =
				InBackVec3(startRot[i], aLookat.ToEuler(), interpolationTimer.GetTimeRate());
		}
	}
	else if (interpolationTimer.GetEnd())
	{
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
}