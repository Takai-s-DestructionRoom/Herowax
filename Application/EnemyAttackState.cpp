#include "EnemyAttackState.h"
#include "Enemy.h"
#include "InstantDrawer.h"

EnemyFindState::EnemyFindState()
{
	lifeTimer.Start();
	isStart = true;
}

void EnemyFindState::Update(Enemy* enemy)
{
	if (isStart) {
		position = enemy->obj.mTransform.position;
		startY = position.y;
		endY = position.y + enemy->GetScale().y * 1.5f;
	}
	position.x = enemy->obj.mTransform.position.x;
	position.z = enemy->obj.mTransform.position.z;

	position.y = Easing::OutBack(startY, endY,lifeTimer.GetTimeRate());

	lifeTimer.Update();
	//びっくりを出す(びっくり出す時間とこのステートの生存時間は同じ)
	InstantDrawer::DrawGraph3D(position,5.0f,5.0f,"exclametion");

	//時間たったら次へ
	if (lifeTimer.GetEnd()) {
		//遷移命令
		enemy->ChangeAttackState<EnemyPreState>();
	}
}

EnemyPreState::EnemyPreState()
{
	lifeTimer.Start();
}

void EnemyPreState::Update(Enemy* enemy)
{
	lifeTimer.Update();
	//予測線描画コマンド送信
	
	//時間たったら次へ
	if (lifeTimer.GetEnd()) {
		//遷移命令
		enemy->ChangeAttackState<EnemyNowAttackState>();
	}
}

EnemyNowAttackState::EnemyNowAttackState()
{
	isStart = true;
	chargeTimer.Start();
}

void EnemyNowAttackState::Update(Enemy* enemy)
{
	//初回フレーム時にパーティクル発生
	if (isStart) {
		isStart = false;
	}

	chargeTimer.Update();
	postureTimer.Update();

	//姿勢をずらす
	
	//突っ込むタイマーが終わったら姿勢修正タイマー開始
	if (chargeTimer.GetNowEnd()) {
		postureTimer.Start();
	}

	if (postureTimer.GetEnd()) {
		//遷移命令
		enemy->ChangeAttackState<EnemyNonAttackState>();
	}
}

void EnemyNonAttackState::Update(Enemy* enemy)
{
	//何もしない
	enemy;
}

void EnemyAttackState::LoadResource()
{
	TextureManager::Load("./Resources/exclametion.png", "exclametion");
}
