#include "BossPunch.h"
#include "Boss.h"

BossPunch::BossPunch(bool isLeft)
{
	punchTimer = 0.7f;
	punchTimer.Reset();

	isLeft_ = isLeft;
}

void BossPunch::Update(Boss* boss)
{
	boss->SetStateStr("Punch");	//ステートがわかるように設定しとく
	punchTimer.Update();

	if (punchTimer.GetStarted() == false)
	{
		punchTimer.Start();

		//こぶしから目標までのベクトル
		handToTarget =
			(boss->GetTarget()->mTransform.position -
				boss->parts[(size_t)isLeft_].mTransform.position).Normalize();

		splinePoints = {
			boss->parts[(size_t)isLeft_].mTransform.position,					//元の座標から
			boss->parts[(size_t)isLeft_].mTransform.position - handToTarget * 15.f,	//後ろに引いて
			boss->GetTarget()->mTransform.position				//目標地点まで
		};
	}

	//スプライン使って飛ばす
	boss->parts[(size_t)isLeft_].mTransform.position =
		Util::Spline(splinePoints, Easing::InQuad(punchTimer.GetTimeRate()));

	handToTarget.Normalize();
	//ターゲットの方向を向いてくれる
	Quaternion aLookat = Quaternion::LookAt(handToTarget);

	//殴り時のモーション遷移
	punchRadianX = Easing::InQuad(Util::AngleToRadian(-60.f), 0, punchTimer.GetTimeRate());
	if (isLeft_)
	{
		punchRadianY = Easing::InQuad(Util::AngleToRadian(90.f), 0, punchTimer.GetTimeRate());
	}
	else
	{
		punchRadianY = Easing::InQuad(Util::AngleToRadian(-90.f), 0, punchTimer.GetTimeRate());
	}

	Quaternion punchQuaterX = Quaternion::AngleAxis({ 1,0,0 }, punchRadianX);
	Quaternion punchQuaterY = Quaternion::AngleAxis({ 0,1,0 }, punchRadianY);

	//計算した向きをかける
	aLookat = aLookat * punchQuaterX * punchQuaterY;

	//euler軸へ変換
	boss->parts[(size_t)isLeft_].mTransform.rotation = aLookat.ToEuler();
	boss->obj.mTransform.rotation = aLookat.ToEuler();
}