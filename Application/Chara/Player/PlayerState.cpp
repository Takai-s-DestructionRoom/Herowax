#include "PlayerState.h"
#include "Player.h"
#include "RInput.h"
#include "WaxManager.h"
#include "Camera.h"
#include "Quaternion.h"

void PlayerNormal::Update(Player* player)
{
	//カメラの向いている方向を取得
	Vector3 camVec = Camera::sNowCamera->mViewProjection.mTarget -
		Camera::sNowCamera->mViewProjection.mEye;
	camVec.Normalize();
	camVec.y = 0;

	//ターゲットの方向を向いてくれる
	Quaternion aLookat = Quaternion::LookAt(camVec);

	//euler軸へ変換
	player->obj.mTransform.rotation = aLookat.ToEuler();

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

	Vector2 stick = RInput::GetInstance()->GetPadLStick();
	//カメラから注視点へのベクトル
	Vector3 cameraVec = Camera::sNowCamera->mViewProjection.mTarget - 
		Camera::sNowCamera->mViewProjection.mEye;
	cameraVec.y = 0;
	cameraVec.Normalize();
	//カメラの角度
	float cameraRad = atan2f(cameraVec.x, cameraVec.z);
	//スティックの角度
	float stickRad = atan2f(stick.x, stick.y);

	Vector3 shotVec = { 0, 0, 1 };									//正面を基準に
	shotVec *= Matrix4::RotationY(cameraRad + stickRad);	//カメラの角度から更にスティックの入力角度を足して
	shotVec.Normalize();									//方向だけの情報なので正規化して
	shotVec *= stick.LengthSq();							//傾き具合を大きさに反映

	//ターゲットの方向を向いてくれる
	Quaternion aLookat = Quaternion::LookAt(shotVec);

	//euler軸へ変換
	player->obj.mTransform.rotation = aLookat.ToEuler();

	//パッドの入力があったらそっちの方向を正面として飛ばす
	//弾きの入力があったら生成
	if (abs(oldLength) < player->shotDeadZone &&
		abs(RInput::GetInstance()->GetPadLStick().LengthSq()) >= player->shotDeadZone)
	{
		player->PabloAttack();
	}

	oldLength = RInput::GetInstance()->GetPadLStick().LengthSq();
}