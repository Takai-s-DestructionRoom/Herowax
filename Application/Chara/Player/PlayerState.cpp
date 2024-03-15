#include "PlayerState.h"
#include "Player.h"
#include "RInput.h"
#include "WaxManager.h"

void PlayerNormal::Update(Player* player)
{
	//入力があればステート遷移
	if (RInput::GetInstance()->GetRTrigger()) {
		player->attackState = std::make_unique<PlayerPablo>();
		//通常の移動速度に戻す
		player->moveSpeed *= player->pabloSpeedMag;

		//押し込んだ時に一回攻撃を行う
		player->PabloAttack();
	}

	player->Attack();
}

void PlayerPablo::Update(Player* player)
{
	//入力があればステート遷移
	if (!RInput::GetInstance()->GetRTrigger()) {
		player->attackState = std::make_unique<PlayerNormal>();
		//通常の移動速度に戻す
		player->moveSpeed /= player->pabloSpeedMag;
	}

	//パッドの入力があったらそっちの方向を正面として飛ばす
	//弾きの入力があったら生成
	if (abs(oldLength) < player->shotDeadZone &&
		abs(RInput::GetInstance()->GetPadLStick().LengthSq()) >= player->shotDeadZone)
	{
		player->PabloAttack();
	}

	oldLength = RInput::GetInstance()->GetPadLStick().LengthSq();
}