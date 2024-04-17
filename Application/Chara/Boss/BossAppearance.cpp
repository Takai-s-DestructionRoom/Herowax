#include "BossAppearance.h"
#include "Boss.h"

BossAppearance::BossAppearance()
{
	priority = 0;
}

void BossAppearance::Update(Boss* boss)
{
	boss->SetStateStr("Appearance");	//ステートがわかるように設定しとく
	handEaseTimer.Update();

	if (isStart)
	{
		handEaseTimer.Start();

		//こぶしから目標までのベクトル
		for (size_t i = 0; i < handToTarget.size(); i++)
		{
			handToTarget[i] =
				(Camera::sNowCamera->mViewProjection.mEye -
					boss->parts[i].oriPos).Normalize();

			Vector3 sideVec{};
			if (i == 0)
			{
				sideVec = Vector3(-1, 0, 0);
			}
			else
			{
				sideVec = Vector3(1, 0, 0);
			}

			splinePoints[i] = {
				boss->parts[i].oriPos,					//元の座標から
				boss->parts[i].oriPos + (boss->GetFrontVec() * sideVec * 20.f),		//横に膨らませる
				Camera::sNowCamera->mViewProjection.mEye - handToTarget[i] * 50.f	//目標地点まで
			};
		}

		isStart = false;
	}

	//スプライン使って飛ばす
	for (size_t i = 0; i < handToTarget.size(); i++)
	{
		boss->parts[i].obj.mTransform.position =
			Util::Spline(splinePoints[i], Easing::InQuad(handEaseTimer.GetTimeRate()));

		handToTarget[i].Normalize();
	}

	//カメラの向く
	Vector3 vec = Camera::sNowCamera->mViewProjection.mEye - boss->obj.mTransform.position;
	vec.Normalize();
	vec.y = 0;

	//ターゲットの方向を向いてくれる
	Quaternion lookat = Quaternion::LookAt(vec);

	//euler軸へ変換
	boss->obj.mTransform.rotation = lookat.ToEuler();

	for (size_t i = 0; i < boss->parts.size(); i++)
	{
		//ターゲットの方向を向いてくれる
		Quaternion lookatHand = Quaternion::LookAt(handToTarget[i]);

		boss->parts[i].obj.mTransform.rotation = lookatHand.ToEuler();
	}
}
