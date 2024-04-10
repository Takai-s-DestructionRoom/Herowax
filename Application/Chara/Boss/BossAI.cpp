#include "BossAI.h"
#include "Boss.h"
#include "BossNormal.h"
#include "BossPunch.h"
#include "BossPunch.h"
#include "TimeManager.h"

void BossAI::Init()
{
	isStart = true;
}

void BossAI::Update(Boss* boss)
{
	bool changeAct = false;
	bool isStand = false;

	if (isStart)
	{
		boss->standTimer.Reset();
		isStart = false;
	}

	//ボスが待機モーション中なら
	if (boss->GetStateStr() == "Normal") {
		boss->standTimer.Update();

		if (boss->standTimer.GetStarted() == false) {
			boss->standTimer.Start();
		}

		//待機時間過ぎたら次の行動へ
		if (boss->standTimer.GetEnd()) {
			changeAct = true;
			isStand = true;
		}
	}

	//ボスの現在の行動が終わっているなら
	if (boss->state->GetIsFinished()) {
		//次の行動へ
		changeAct = true;
	}

	if (changeAct) {
		boss->standTimer.Reset();

		//待機モーションから遷移した場合
		if (isStand)
		{
			int32_t num = Util::GetRand(1, 100);	//確率用

			//パンチに移行
			//50%で右パンチ50%で左パンチ
			if (num > 50)
			{
				boss->state = std::make_unique<BossPunch>(true);
			}
			else
			{
				boss->state = std::make_unique<BossPunch>(false);
			}
			return;
		}
		//何かしらの行動後
		else
		{
			//どれにも該当しなさそうなので待機モーションへ
			boss->state = std::make_unique<BossNormal>();
			return;
		}
	}
}
