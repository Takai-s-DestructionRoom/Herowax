#include "BossAppearance.h"
#include "Boss.h"

BossAppearance::BossAppearance()
{
	priority = 0;
	isStart = true;
}

void BossAppearance::Update(Boss* boss)
{
	boss->SetStateStr("Appearance");	//ステートがわかるように設定しとく
	handEaseTimer.Update();
	waitTimer.Update();

	//カメラの向く
	Vector3 vec = Camera::sNowCamera->mViewProjection.mEye - boss->obj.mTransform.position;
	vec.Normalize();
	vec.y = 0;

	//ターゲットの方向を向いてくれる
	Quaternion lookat = Quaternion::LookAt(vec);

	//euler軸へ変換
	boss->obj.mTransform.rotation = lookat.ToEuler();

	if (isStart)
	{
		waitTimer.Start();

		//こぶしから目標までのベクトル
		for (size_t i = 0; i < handToTarget.size(); i++)
		{
			handToTarget[i] =
				(Camera::sNowCamera->mViewProjection.mEye -
					boss->parts[i].oriPos).Normalize();

			Vector3 sideVec{};
			if (i == 0)
			{
				sideVec = boss->GetFrontVec().Cross(Vector3(0, -1, 0));
			}
			else
			{
				sideVec = boss->GetFrontVec().Cross(Vector3(0, 1, 0));
			}

			splinePoints[i] = {
				boss->parts[i].oriPos,					//元の座標から
				boss->parts[i].oriPos + (sideVec * 30.f),		//横に膨らませる
				Camera::sNowCamera->mViewProjection.mEye - handToTarget[i] * 50.f	//目標地点まで
			};
		}

		isStart = false;
	}

	if (waitTimer.GetEnd() && handEaseTimer.GetStarted() == false)
	{
		handEaseTimer.Start();
	}

	//スプライン使って飛ばす
	for (size_t i = 0; i < handToTarget.size(); i++)
	{
		boss->parts[i].obj.mTransform.position =
			Util::Spline(splinePoints[i], Easing::InQuad(handEaseTimer.GetTimeRate()));

		handToTarget[i].Normalize();
	}

	for (size_t i = 0; i < boss->parts.size(); i++)
	{
		//ターゲットの方向を向いてくれる
		Quaternion lookatHand = Quaternion::LookAt(handToTarget[i]);

		boss->parts[i].obj.mTransform.rotation = lookatHand.ToEuler();
	}
}
