#include "BossAI.h"
#include "Boss.h"
#include "BossNormal.h"
#include "BossPunch.h"
#include "BossFallAttack.h"
#include "TimeManager.h"
#include "Level.h"
#include "RImGui.h"

void BossAI::Init()
{
	isStart = true;
	situation = BossSituation::All;

	//壁までの距離
	maxDist = Level::Get()->wall[0].mTransform.position.Length();
	//ボスとプレイヤーの距離
	dist = 0;
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

	if (situation == BossSituation::Appearance) {
		boss->state = std::make_unique<BossNormal>();
	}

	if (changeAct) {
		boss->standTimer.Reset();

		//待機モーションから遷移した場合
		if (isStand)
		{
			int32_t num = Util::GetRand(1, 100);	//確率用
			//ボスとプレイヤーの距離
			dist = (boss->GetTarget()->mTransform.position - boss->obj.mTransform.position).Length();

			if (situation == BossSituation::All) {
				//パンチに移行
				if (dist / maxDist < 0.6f)	//距離近ければ90%パンチ10%落下攻撃
				{
					if (num < 45)
					{
						boss->state = std::make_unique<BossPunch>(true);
					}
					else if (num < 90)
					{
						boss->state = std::make_unique<BossPunch>(false);
					}
					else
					{
						boss->state = std::make_unique<BossFallAttack>();
					}
				}
				else						//距離遠ければ60%パンチ40%落下攻撃
				{
					if (num < 30)
					{
						boss->state = std::make_unique<BossPunch>(true);
					}
					else if (num < 60)
					{
						boss->state = std::make_unique<BossPunch>(false);
					}
					else
					{
						boss->state = std::make_unique<BossFallAttack>();
					}
				}
				return;
			}
			//左しか残ってないなら
			if (situation == BossSituation::OnlyLeft) {
				if (dist / maxDist < 0.6f)	//距離近ければ90%パンチ10%落下攻撃
				{
					if (num < 90)
					{
						boss->state = std::make_unique<BossPunch>(true);
					}
					else
					{
						boss->state = std::make_unique<BossFallAttack>();
					}
				}
				else						//距離遠ければ60%パンチ40%落下攻撃
				{
					if (num < 60)
					{
						boss->state = std::make_unique<BossPunch>(true);
					}
					else
					{
						boss->state = std::make_unique<BossFallAttack>();
					}
				}
				return;
			}
			//右しか残ってないなら
			if (situation == BossSituation::OnlyRight) {
				if (dist / maxDist < 0.6f)	//距離近ければ90%パンチ10%落下攻撃
				{
					if (num < 90)
					{
						boss->state = std::make_unique<BossPunch>(false);
					}
					else
					{
						boss->state = std::make_unique<BossFallAttack>();
					}
				}
				else						//距離遠ければ60%パンチ40%落下攻撃
				{
					if (num < 60)
					{
						boss->state = std::make_unique<BossPunch>(false);
					}
					else
					{
						boss->state = std::make_unique<BossFallAttack>();
					}
				}
				return;
			}
			//腕が無いなら落下攻撃
			if (situation == BossSituation::NoArms) {
				boss->state = std::make_unique<BossFallAttack>();
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

	if (RImGui::showImGui)
	{
		ImGui::SetNextWindowSize({ 600, 250 }, ImGuiCond_FirstUseEver);

		ImGui::Begin("ボスAI");
		ImGui::Text("距離割合:%f", dist / maxDist);
		ImGui::Text("ボスとの距離:%f", dist);
		ImGui::Text("最大距離:%f", maxDist);

		ImGui::End();
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
