#include "EnemyRangeAttackState.h"
#include "EnemyAttackState.h"
#include "Enemy.h"
#include "EnemyManager.h"

EnemyRangePreState::EnemyRangePreState()
{
	lifeTimer.Start();
	blinkTimer.Start();
}

void EnemyRangePreState::Update(Enemy* enemy)
{
	enemy->SetAttackStateStr(EnemyRangePreState::GetStateStr());

	ModelObj* target = enemy->GetTarget();
	Vector3 pVec = target->mTransform.position - enemy->obj.mTransform.position;
	pVec.Normalize();
	pVec.y = 0;

	lifeTimer.Update();
	blinkTimer.RoopReverse();

	//予測線送信
	//トランスフォームはプレイヤー基準に
	float baseAlpha = 0.5f;
	enemy->predictionLine.mTuneMaterial.mColor = Color(1.f, 0.f, 0.f, baseAlpha);
	enemy->predictionLine.mTuneMaterial.mColor.a = baseAlpha * blinkTimer.GetTimeRate() + 0.2f;
	enemy->predictionLine.mTransform = enemy->obj.mTransform;
	enemy->predictionLine.mTransform.rotation.x = 0;

	float xSize = enemy->obj.mTransform.scale.x;
	enemy->predictionLine.mTransform.scale = { xSize,0.1f,enemy->attackMovePower };
	//大きさ分前に置く
	enemy->predictionLine.mTransform.position += pVec * enemy->attackMovePower * 0.5f;

	//角度加算
	//向く方向のベクトルを取得
	Vector3 aVec = pVec;
	aVec.Normalize();
	aVec.y = 0;

	//ターゲットの方向を向いてくれる
	Quaternion aLookat = Quaternion::LookAt(aVec);

	//euler軸へ変換
	enemy->RotVecPlus(aLookat.ToEuler());
	
	//時間たったら次へ
	if (lifeTimer.GetEnd()) {
		//遷移命令
		enemy->ChangeAttackState<EnemyRangeNowState>();
	}
}

std::string EnemyRangePreState::GetStateStr()
{
	return "RangePre";
}

EnemyRangeNowState::EnemyRangeNowState()
{
}

void EnemyRangeNowState::Update(Enemy* enemy)
{
	enemy->SetAttackStateStr(EnemyRangeNowState::GetStateStr());

	//球を撃たせる
	EnemyManager::GetInstance()->CreateEnemyShot(enemy);
	//ステート変更
	enemy->ChangeAttackState<EnemyNormalState>();
}

std::string EnemyRangeNowState::GetStateStr()
{
	return "RangeNow";
}
