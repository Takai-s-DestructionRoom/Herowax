#include "BossPunch.h"
#include "Boss.h"

BossPunch::BossPunch()
{
}

BossPunch::BossPunch(bool isLeft)
{
	isFinished = false;

	isLeft_ = isLeft;
	isStart = true;
}

void BossPunch::Update(Boss* boss)
{
	boss->SetStateStr("Punch");	//ステートがわかるように設定しとく
	boss->punchTimer.Update();
	boss->punchStayTimer.Update();

	if (isStart)
	{
		boss->punchTimer.Start();

		//こぶしから目標までのベクトル
		handToTarget =
			(boss->GetTarget()->mTransform.position -
				boss->parts[(size_t)isLeft_].obj.mTransform.position).Normalize();

		splinePoints = {
			boss->parts[(size_t)isLeft_].obj.mTransform.position,					//元の座標から
			boss->parts[(size_t)isLeft_].obj.mTransform.position - handToTarget * 15.f,	//後ろに引いて
			boss->GetTarget()->mTransform.position				//目標地点まで
		};

		isStart = false;
	}

	//スプライン使って飛ばす
	boss->parts[(size_t)isLeft_].obj.mTransform.position =
		Util::Spline(splinePoints, Easing::InQuad(boss->punchTimer.GetTimeRate()));

	handToTarget.Normalize();
	//ターゲットの方向を向いてくれる
	Quaternion aLookat = Quaternion::LookAt(handToTarget);

	//殴り時のモーション遷移
	punchRadianX = Easing::InQuad(Util::AngleToRadian(-60.f), 0, boss->punchTimer.GetTimeRate());
	if (isLeft_)
	{
		punchRadianY = Easing::InQuad(Util::AngleToRadian(90.f), 0, boss->punchTimer.GetTimeRate());
	}
	else
	{
		punchRadianY = Easing::InQuad(Util::AngleToRadian(-90.f), 0, boss->punchTimer.GetTimeRate());
	}

	Quaternion punchQuaterX = Quaternion::AngleAxis({ 1,0,0 }, punchRadianX);
	Quaternion punchQuaterY = Quaternion::AngleAxis({ 0,1,0 }, punchRadianY);

	//計算した向きをかける
	aLookat = aLookat * punchQuaterX * punchQuaterY;

	//euler軸へ変換
	boss->parts[(size_t)isLeft_].obj.mTransform.rotation = aLookat.ToEuler();
	boss->obj.mTransform.rotation = aLookat.ToEuler();

	if (boss->punchTimer.GetEnd() && boss->punchStayTimer.GetStarted() == false)
	{
		boss->punchStayTimer.Start();
	}

	if (boss->punchStayTimer.GetEnd())
	{
		isFinished = true;
		boss->punchTimer.Reset();
		boss->punchStayTimer.Reset();
	}
}