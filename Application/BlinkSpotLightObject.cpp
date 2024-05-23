#include "BlinkSpotLightObject.h"
#include "Util.h"

//タイマーが終わったら、チカチカを開始
	//タイマーはループ(チカチカのクールタイムにする)
	//ループ開始時にタイマーの最大時間をランダムで変える

	//チカチカは別で進める

	//チカチカタイマーを用意
	//3回ループ


void BlinkSpotLightObject::Init()
{
	blinkCoolTimer.maxTime_ = Util::GetRand(COOLTIME_MIN, COOLTIME_MAX);
	blinkCoolTimer.Start();
	
	count = BLINK_COUNT;

	SpotLightObject::Init();
}

void BlinkSpotLightObject::Update()
{
	SpotLightObject::Update();

	if (saveColor.r == -1 &&
		saveColor.g == -1 &&
		saveColor.b == -1) 
	{
		saveColor = obj.mTuneMaterial.mColor;
	}

	if (count >= BLINK_COUNT) {
		blinkCoolTimer.Update();
		
		if (!blinkCoolTimer.GetStarted()) {
			blinkCoolTimer.maxTime_ = Util::GetRand(2.0f,8.0f);
			blinkCoolTimer.Start();
		}
		if (blinkCoolTimer.GetEnd()) {
			count = 0;
			blinkCoolTimer.Reset();
		}
	}

	//カウントが上限を超えたら更新を停止
	if (count < BLINK_COUNT) {
		blinkRoopTimer.Update();
	}

	obj.mTuneMaterial.mColor = saveColor * (1.0f - blinkRoopTimer.GetTimeRate());

	if (blinkRoopTimer.GetStarted() == false && blinkRoopTimer.GetReverseStarted() == false)
	{
		blinkRoopTimer.Start();
	}
	else if (blinkRoopTimer.GetEnd())
	{
		blinkRoopTimer.ReverseStart();
	}
	else if (blinkRoopTimer.GetReverseEnd())
	{
		blinkRoopTimer.Reset();
		//タイマーが終わるタイミングでカウントを加算
		count++;
	}
}

void BlinkSpotLightObject::BlinkStart()
{
	
}
