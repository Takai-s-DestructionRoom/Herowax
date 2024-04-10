#include "BossPart.h"
#include "TimeManager.h"

Parts::Parts() : GameObject()
{
	colliderSize = 20.f;
}

void Parts::Init()
{
	hp = maxHP;
}

void Parts::Update()
{
	mutekiTimer.Update();
	whiteTimer.Update();
	waxShakeOffTimer.Update();

	//かかっているロウを振り払う
	//10段階かかったら固まる
	if (waxSolidCount > 0 && waxSolidCount < requireWaxSolidCount) {
		//振り払うタイマーが終わったら
		if (waxShakeOffTimer.GetEnd())
		{
			//振り払う
			waxShakeOffTimer.Reset();
			//減るのは一段階
			waxSolidCount--;
		}
	}

	//白を加算
	if (whiteTimer.GetStarted()) {
		brightColor.r = Easing::OutQuad(1.f, 0.f, whiteTimer.GetTimeRate());
		brightColor.g = Easing::OutQuad(1.f, 0.f, whiteTimer.GetTimeRate());
		brightColor.b = Easing::OutQuad(1.f, 0.f, whiteTimer.GetTimeRate());
	}

	UpdateCollider();

	obj.mTransform.UpdateMatrix();
	BrightTransferBuffer(Camera::sNowCamera->mViewProjection);
	obj.mPaintDataBuff->dissolveVal = waxSolidCount >= requireWaxSolidCount ? 1.0f : 0.3f / (requireWaxSolidCount - 1) * waxSolidCount;
	obj.mPaintDataBuff->color = Color(0.8f, 0.6f, 0.35f, 1.0f);
	obj.mPaintDataBuff->slide += TimeManager::deltaTime;
}

void Parts::Draw()
{
	BrightDraw();
}

void Parts::DealDamage(int32_t damage)
{
	//無敵時間さん中ならスキップ
	if (mutekiTimer.GetRun())return;

	//無敵時間開始
	mutekiTimer.Start();

	//無敵時間の前半の時間は白く光る演出
	whiteTimer.maxTime_ = mutekiTimer.maxTime_ / 2;
	whiteTimer.Start();

	//かかりカウント加算
	waxSolidCount++;
	//振り払いタイマー開始
	//(初期化も行うため、攻撃を受ける度にタイマーが継続する形に)
	waxShakeOffTimer.Start();

	//一応HPにダメージ(使うか不明)
	hp -= damage;
}
