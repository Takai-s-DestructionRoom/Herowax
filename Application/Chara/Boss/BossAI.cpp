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
			if (situation == BossSituation::All) {
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
			//左しか残ってないなら左パンチ
			if (situation == BossSituation::OnlyLeft) {
				boss->state = std::make_unique<BossPunch>(true);
				return;
			}
			//右しか残ってないなら右パンチ
			if (situation == BossSituation::OnlyRight) {
				boss->state = std::make_unique<BossPunch>(false);
				return;
			}
			//腕が無いなら待機モーションへ(今後別モーションへの遷移など実装)
			if (situation == BossSituation::NoArms) {
				boss->state = std::make_unique<BossNormal>();
				return;
			}
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

BossSituation BossAI::GetSituation()
{
	return situation;
}

void BossAI::SetSituation(const BossSituation& situation_)
{
	situation = situation_;
}
