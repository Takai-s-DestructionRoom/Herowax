#include "BossFallAttack.h"
#include "Boss.h"
#include "ParticleManager.h"
#include <RAudio.h>

BossFallAttack::BossFallAttack()
{
	isFinished = false;
	isStart = true;

	maxHight = 100.f;
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
		boss->Shake(boss->fallAtkShoutTimer.maxTime_, 1.f);	//震える

		speed = boss->fallSpeed;

		for (size_t i = 0; i < boss->fallParts.size(); i++)
		{
			//地面より下に
			boss->fallParts[i].mTransform.position.y = -boss->fallParts[i].mTransform.scale.y * 2.f;
			//ランダムに配置
			float dist = Util::GetRand(boss->obj.mTransform.scale.x,boss->fallRange);	//生成距離
			float angle = Util::AngleToRadian(Util::GetRand(0.f,360.f));				//生成角度

			Vector2 pos = Vector2(1, 1).Rotation(angle) * dist;

			boss->fallParts[i].mTransform.position.x = pos.x;
			boss->fallParts[i].mTransform.position.z = pos.y;

			boss->warning[i].mTransform.position = boss->fallParts[i].mTransform.position;
			boss->warning[i].mTransform.position.y = 0.f;

			boss->fallParts[i].mTransform.scale = Vector3(1,1,1) * boss->fallPartsSize;
			boss->warning[i].mTransform.scale = Vector3(1,0.1f,1) * boss->fallPartsSize;
		}

		isStart = false;
	}

	//叫んでる時
	if (boss->fallAtkShoutTimer.GetRun())
	{
		//全てのパーツを上空に
		for (size_t i = 0; i < boss->fallParts.size(); i++)
		{
			boss->fallParts[i].mTransform.position.y =
				Easing::OutBack(
					-boss->fallParts[i].mTransform.scale.y * 2.f,
					maxHight,
					boss->fallAtkShoutTimer.GetTimeRate());
		}
	}
	//叫び終わったら
	else if (boss->fallAtkShoutTimer.GetEnd() && boss->fallAtkTimer.GetStarted() == false)
	{
		boss->fallAtkTimer.Start();
	}

	//攻撃中
	if (boss->fallAtkTimer.GetRun())
	{
		speed += boss->fallAccel;

		for (size_t i = 0; i < boss->fallParts.size(); i++)
		{
			boss->fallParts[i].mTransform.position.y -= speed;

			//警告も一緒に落ちてく
			if (boss->warning[i].mTransform.position.y > boss->fallParts[i].mTransform.position.y)
			{
				boss->warning[i].mTransform.position.y = boss->fallParts[i].mTransform.position.y;
			}
		}
	}
	//攻撃終わったら
	else if (boss->fallAtkTimer.GetEnd() && boss->fallAtkStayTimer.GetStarted() == false)
	{
		boss->fallAtkStayTimer.Start();
	}

	//待機時間終わったら終了フラグ立てる
	if (boss->fallAtkStayTimer.GetEnd())
	{
		isFinished = true;
		boss->fallAtkTimer.Reset();
		boss->fallAtkStayTimer.Reset();
	}

	for (size_t i = 0; i < boss->fallParts.size(); i++)
	{
		boss->fallParts[i].mTransform.UpdateMatrix();
		boss->fallParts[i].TransferBuffer(Camera::sNowCamera->mViewProjection);
		boss->warning[i].mTransform.UpdateMatrix();
		boss->warning[i].TransferBuffer(Camera::sNowCamera->mViewProjection);
	}
}
