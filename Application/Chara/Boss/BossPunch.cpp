#include "BossPunch.h"
#include "Boss.h"
#include "ParticleManager.h"
#include <RAudio.h>

BossPunch::BossPunch(bool isLeft)
{
	RAudio::Load("Resources/Sounds/SE/E_punch.wav", "Punch");
	RAudio::Load("Resources/Sounds/SE/E_attention.wav", "Attention");

	isFinished = false;

	isLeft_ = isLeft;
	isStart = true;

	RAudio::Load("Resources/Sounds/SE/E_punch.wav", "Punch");
	RAudio::Load("Resources/Sounds/SE/E_attention.wav", "Attention");
}

void BossPunch::Update(Boss* boss)
{
	boss->SetStateStr("Punch");	//ステートがわかるように設定しとく
	boss->punchTimer.Update();
	boss->punchStayTimer.Update();

	if (isStart)
	{
		boss->punchTimer.Reset();
		boss->punchStayTimer.Reset();

		boss->punchTimer.Start();

		//こぶしから目標までのベクトル
		handToTarget =
			(boss->GetTarget()->mTransform.position -
				boss->parts[(size_t)isLeft_].obj.mTransform.position).Normalize();

		splinePoints = {
			boss->parts[(size_t)isLeft_].obj.mTransform.position,					//元の座標から
			boss->parts[(size_t)isLeft_].obj.mTransform.position - handToTarget * 15.f,	//後ろに引いて
			//boss->GetFrontVec() * Vector3((float)isLeft_,0,0) * 30.f,	//横に膨らませる
			boss->GetTarget()->mTransform.position				//目標地点まで
		};

		//スプラインの終点に攻撃マーク表示
		boss->targetCircle.mTransform.position = splinePoints.back();
		RAudio::Play("Attention");

		//ちょい浮かせる
		boss->targetCircle.mTransform.rotation.y = 0.02f;

		//当たり判定に合わせる
		boss->targetCircle.mTransform.scale =
		{ boss->parts[(size_t)isLeft_].collider.r,
			1.f,
			boss->parts[(size_t)isLeft_].collider.r };

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
		punchRadianY = Easing::InQuad(Util::AngleToRadian(-90.f), 0, boss->punchTimer.GetTimeRate());
	}
	else
	{
		punchRadianY = Easing::InQuad(Util::AngleToRadian(90.f), 0, boss->punchTimer.GetTimeRate());
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
		RAudio::Play("Punch");

		boss->punchStayTimer.Start();

		//エミッターの座標はプレイヤーの座標からY座標だけにスケール分ずらしたもの
		Vector3 emitterPos = boss->parts[(size_t)isLeft_].obj.mTransform.position;
		emitterPos.y = 0.f;

		//パーティクル生成
		ParticleManager::GetInstance()->AddRing(emitterPos, "punch_impact_ring");
	}

	//どっちかの手が固まってるなら
	if ((boss->parts[0].GetIsSolid() && boss->parts[0].GetIsAlive()) ||
		(boss->parts[1].GetIsSolid() && boss->parts[1].GetIsAlive()))
	{
		//ステート変化を通らなくさせる
		return;
	}

	if (boss->punchStayTimer.GetEnd())
	{
		isFinished = true;
		boss->punchTimer.Reset();
		boss->punchStayTimer.Reset();
	}
}