#include "EnemyAttackState.h"
#include "Enemy.h"
#include "InstantDrawer.h"
#include "Camera.h"

EnemyFindState::EnemyFindState()
{
	lifeTimer.Start();
	isStart = true;
}

void EnemyFindState::Update(Enemy* enemy)
{
	enemy->SetAttackStateStr("Find");

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

	blinkTimer.maxTime_ = lifeTimer.maxTime_ / 4;
	blinkTimer.Start();

	isStart = true;
}

void EnemyPreState::Update(Enemy* enemy)
{
	ModelObj* target = enemy->GetTarget();
	Vector3 pVec = target->mTransform.position - enemy->obj.mTransform.position;
	pVec.Normalize();

	if (isStart) {
		isStart = false;
		start = enemy->obj.mTransform.position;
		end = enemy->obj.mTransform.position - pVec * 10.f;
	}
	enemy->SetAttackStateStr("Pre");

	float oldTime = lifeTimer.GetTimeRate();
	lifeTimer.Update();
	blinkTimer.RoopReverse();
	
	//予測線描画コマンド送信
	//トランスフォームはプレイヤー基準に
	float baseAlpha = 0.5f;
	enemy->predictionLine.mTuneMaterial.mColor = Color(1.f, 0.f, 0.f, baseAlpha);
	enemy->predictionLine.mTuneMaterial.mColor.a = baseAlpha * blinkTimer.GetTimeRate() + 0.2f;
	enemy->predictionLine.mTransform = enemy->obj.mTransform;
	float xSize = enemy->obj.mTransform.scale.x;
	enemy->predictionLine.mTransform.scale = { xSize,0.1f,enemy->attackMovePower };
	//大きさ分前に置く
	enemy->predictionLine.mTransform.position += enemy->GetFrontVec() *
		enemy->attackMovePower * 0.5f;
	
	//ちょっと下げる
	Vector3 nowMove = OutQuadVec3(start, end, lifeTimer.GetTimeRate());
	nowMove.y = 0;
	Vector3 oldMove = OutQuadVec3(start, end, oldTime);
	oldMove.y = 0;

	Vector3 plusVec = nowMove - oldMove;
	enemy->MoveVecPlus(plusVec);

	//時間たったら次へ
	if (lifeTimer.GetEnd()) {
		//遷移命令
		enemy->ChangeAttackState<EnemyNowAttackState>();
		
		//突撃位置更新終了(ターゲット更新)
		//この時点でのターゲットの座標をコピーして、そこへ突撃する
		enemy->attackStartPos = enemy->obj.mTransform.position;
		enemy->attackEndPos = enemy->attackStartPos + pVec * enemy->attackMovePower;
	}
}

EnemyNowAttackState::EnemyNowAttackState()
{
	isStart = true;
	chargeTimer.Start();
}

void EnemyNowAttackState::Update(Enemy* enemy)
{
	enemy->SetAttackStateStr("NowAttack");

	//初回フレーム時にパーティクル発生
	if (isStart) {
		isStart = false;
	}

	oldChargeTime = chargeTimer.GetTimeRate();
	chargeTimer.Update();
	postureTimer.Update();

	//突撃させる(移動量に足したい)
	//いったん地面に沿うように
	Vector3 nowMove = OutQuadVec3(enemy->attackStartPos, enemy->attackEndPos,chargeTimer.GetTimeRate());
	nowMove.y = 0;
	Vector3 oldMove = OutQuadVec3(enemy->attackStartPos, enemy->attackEndPos, oldChargeTime);
	oldMove.y = 0;

	enemy->MoveVecPlus(nowMove - oldMove);

	//プレイヤーに向かって移動するAI
	Vector3 aVec = enemy->GetTarget()->mTransform.position - enemy->obj.mTransform.position;
	aVec.Normalize();
	aVec.y = 0;

	//ターゲットの方向を向いてくれる
	Quaternion aLookat = Quaternion::LookAt(aVec);

	//前30度へ傾く(これだと特定方向に傾いてしまうから変更する)
	float radStartZ = Util::AngleToRadian(-30.f);

	//喰らい時のモーション遷移
	float radZ = Easing::InQuad(radStartZ,0,postureTimer.GetTimeRate());

	//正面の横ベクトルを軸に回転
	aVec.Cross({ 1,0,1 });
	Quaternion knockQuaterZ = Quaternion::AngleAxis(aVec, radZ);

	//計算した向きをかける
	aLookat = aLookat  * knockQuaterZ;

	//euler軸へ変換
	enemy->obj.mTransform.rotation = aLookat.ToEuler();

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
	enemy->SetAttackStateStr("NonAttack");
}

void EnemyAttackState::LoadResource()
{
	TextureManager::Load("./Resources/exclametion.png", "exclametion");
}
