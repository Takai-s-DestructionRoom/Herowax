#include "PlayerState.h"
#include "Player.h"
#include "RInput.h"
#include "WaxManager.h"
#include "Camera.h"
#include "Quaternion.h"

void PlayerNormal::Update(Player* player)
{
	////カメラの向いている方向を取得
	//Vector3 camVec = Camera::sNowCamera->mViewProjection.mTarget -
	//	Camera::sNowCamera->mViewProjection.mEye;
	//camVec.Normalize();
	//camVec.y = 0;

	////ターゲットの方向を向いてくれる
	//Quaternion aLookat = Quaternion::LookAt(camVec);

	////euler軸へ変換
	//player->obj.mTransform.rotation = aLookat.ToEuler();

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

	//RTを押している間出続ける
	player->PabloAttack();

	oldLength = RInput::GetInstance()->GetPadLStick().LengthSq();
}