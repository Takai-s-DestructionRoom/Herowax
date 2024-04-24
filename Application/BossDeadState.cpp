#include "BossDeadState.h"
#include "Boss.h"
#include "SceneManager.h"
#include "SimpleSceneTransition.h"
#include "ResultScene.h"
#include "EventCaller.h"
#include "BossDeadScene.h"

BossDeadState::BossDeadState()
{
	isStart = true;
	absorpTimer.Start();
}

void BossDeadState::Update(Boss* boss)
{
	boss->SetStateStr("Dead");

	if (isStart) {
		startPos = boss->GetPos();
		startScale = boss->GetScale();
		startRota = boss->obj.mTransform.rotation;
		endRota = startRota + Vector3(0, Util::AngleToRadian(360) * 2, 0);//2回転
		isStart = false;
	}

	//吸収されて死ぬ
	absorpTimer.Update();

	boss->SetPos(InQuadVec3(startPos, boss->collectPos, absorpTimer.GetTimeRate()));
	boss->SetScale(InQuadVec3(startScale, { 0,0,0 }, absorpTimer.GetTimeRate()));
	boss->SetRota(InQuadVec3(startRota, endRota, absorpTimer.GetTimeRate()));

	if (absorpTimer.GetNowEnd()) {
		//死亡
		boss->isAlive = false;
		//シーン切り替え
		//SceneManager::GetInstance()->Change<ResultScene>();
	}
}
