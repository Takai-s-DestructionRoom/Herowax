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
	}


	if (player->isAttack == false || player->isMugenAttack)
	{
		if ((RInput::GetInstance()->GetPadButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) ||
			RInput::GetInstance()->GetMouseClick(1)) &&
			!player->atkCoolTimer.GetRun())
		{
			player->isAttack = true;
			player->atkTimer.Start();

			//入力時の出現位置と方向を記録
			player->atkVec = player->GetFrontVec();
			player->atkVec.y = player->atkHeight;

			//生成
			WaxManager::GetInstance()->Create(
				player->obj.mTransform, player->atkPower, 
				player->atkVec, player->atkSpeed,
				player->atkRange, player->atkSize, 
				player->atkTimer.maxTime_, player->solidTimer.maxTime_);

			//すぐ攻撃のクールタイム始まるように
			if (player->isMugenAttack)
			{
				player->atkCoolTimer.Start();
				player->atkTimer.Reset();
			}
		}
	}

	//攻撃中は
	player->atkTimer.Update();
	player->atkCoolTimer.Update();

	//攻撃終わったらクールタイム開始
	if (player->atkTimer.GetEnd())
	{
		player->atkCoolTimer.Start();
		player->atkTimer.Reset();
	}

	//クールタイム終わったら攻撃再びできるように
	if (player->atkCoolTimer.GetEnd())
	{
		player->isAttack = false;
		player->atkCoolTimer.Reset();
	}
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
		Transform spawnTrans = player->obj.mTransform;

		//プレイヤーの向いている方向*これ
		Vector3 pabloVec = Vector3(RInput::GetInstance()->GetPadLStick().x,
			player->atkHeight,
			RInput::GetInstance()->GetPadLStick().y);

		player->atkVec = pabloVec;

		WaxManager::GetInstance()->Create(
			spawnTrans, player->atkPower,
			player->atkVec, player->atkSpeed,
			player->atkRange, player->atkSize,
			player->atkTimer.maxTime_, player->solidTimer.maxTime_);
	}

	oldLength = RInput::GetInstance()->GetPadLStick().LengthSq();
}