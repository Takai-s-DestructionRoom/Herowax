#include "BossFallAttack.h"
#include "Boss.h"
#include "ParticleManager.h"
#include <RAudio.h>

BossFallAttack::BossFallAttack()
{
	isFinished = false;
	isStart = true;
}

void BossFallAttack::Update(Boss* boss)
{
	boss->SetStateStr("FallAttack");	//ステートがわかるように設定しとく
	boss->fallAtkShoutTimer.Update();
	boss->fallAtkTimer.Update();
	boss->fallAtkStayTimer.Update();

	if (isStart)
	{
		boss->fallAtkShoutTimer.Reset();
		boss->fallAtkTimer.Reset();
		boss->fallAtkStayTimer.Reset();

		boss->fallAtkShoutTimer.Start();

		isStart = false;
	}

	//叫んでる時
	if (boss->fallAtkShoutTimer.GetRun())
	{
		boss->Shake(boss->fallAtkShoutTimer.maxTime_, 1.f);
	}

	//叫び終わったら
	if (boss->fallAtkShoutTimer.GetEnd() && boss->fallAtkTimer.GetStarted() == false)
	{
		boss->fallAtkTimer.Start();
	}

	//攻撃終わったら
	if (boss->fallAtkTimer.GetEnd() && boss->fallAtkStayTimer.GetStarted() == false)
	{
		boss->fallAtkStayTimer.Start();
	}

	for (size_t i = 0; i < boss->fallParts.size(); i++)
	{
		boss->fallParts[i].mTransform.position.y = (float)i * 5.f;


		boss->fallParts[i].mTransform.UpdateMatrix();
		boss->fallParts[i].TransferBuffer(Camera::sNowCamera->mViewProjection);
		boss->warning[i].mTransform.UpdateMatrix();
		boss->warning[i].TransferBuffer(Camera::sNowCamera->mViewProjection);
	}

	//待機時間終わったら終了フラグ立てる
	if (boss->fallAtkStayTimer.GetEnd())
	{
		isFinished = true;
		boss->fallAtkTimer.Reset();
		boss->fallAtkStayTimer.Reset();
	}
}
