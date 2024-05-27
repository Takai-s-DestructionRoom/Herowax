#include "EnemyAttackState.h"
#include "EnemyRangeAttackState.h"
#include "Enemy.h"
#include "InstantDrawer.h"
#include "Camera.h"

void EnemyAttackState::LoadResource()
{
	TextureManager::Load("./Resources/exclametion.png", "exclametion");
}

void EnemyAttackState::ChangeAttackStateString(Enemy* enemy, const std::string& state)
{
	if (state == EnemyPreState::GetStateStr()) {
		enemy->ChangeAttackState<EnemyPreState>();
		//enemy->stateManageNumber++;
	}
	if (state == EnemyRangePreState::GetStateStr()) {
		enemy->ChangeAttackState<EnemyRangePreState>();
		//enemy->stateManageNumber++;
	}
}

void EnemyNormalState::Update(Enemy* enemy)
{
	//何もしない
	enemy->SetAttackStateStr(EnemyNormalState::GetStateStr());
}

std::string EnemyNormalState::GetStateStr()
{
	return "Normal";
}

EnemyFindState::EnemyFindState()
{
	lifeTimer.Start();
	isStart = true;
}

void EnemyFindState::Update(Enemy* enemy)
{
	enemy->SetAttackStateStr(EnemyFindState::GetStateStr());
	
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

	//角度加算
	//向く方向のベクトルを取得
	Vector3 aVec = enemy->GetTarget()->mTransform.position - enemy->GetPos();
	aVec.Normalize();
	aVec.y = 0;

	//ターゲットの方向を向いてくれる
	Quaternion aLookat = Quaternion::LookAt(aVec);
	//euler軸へ変換
	enemy->RotVecPlus(aLookat.ToEuler());

	//時間たったら次へ
	if (lifeTimer.GetEnd()) {
		//遷移命令
		//番号と読みだした文字に応じて変化先を決定
		EnemyAttackState::ChangeAttackStateString(enemy, enemy->stateStrings[enemy->stateManageNumber]);
		//enemy->ChangeAttackState<EnemyPreState>();
	}
}

std::string EnemyFindState::GetStateStr()
{
	return "FindAttack";
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
	enemy->SetAttackStateStr(EnemyPreState::GetStateStr());

	ModelObj* target = enemy->GetTarget();
	Vector3 pVec = target->mTransform.position - enemy->obj.mTransform.position;
	pVec.Normalize();
	pVec.y = 0;

	if (isStart) {
		isStart = false;
		start = enemy->obj.mTransform.position;
		end = enemy->obj.mTransform.position - pVec * 10.f;
	}
	
	float oldTime = lifeTimer.GetTimeRate();
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
	
	//ちょっと下げる
	Vector3 nowMove = OutQuadVec3(start, end, lifeTimer.GetTimeRate());
	nowMove.y = 0;
	Vector3 oldMove = OutQuadVec3(start, end, oldTime);
	oldMove.y = 0;

	//移動量加算
	Vector3 plusVec = nowMove - oldMove;
	enemy->MoveVecPlus(plusVec);

	//角度加算
	//向く方向のベクトルを取得
	Vector3 aVec = pVec;
	aVec.Normalize();
	aVec.y = 0;

	//ターゲットの方向を向いてくれる
	Quaternion aLookat = Quaternion::LookAt(aVec);

	//前30度へ傾く
	float radStartX = Util::AngleToRadian(30.f);

	//モーション遷移
	float radX = Easing::InQuad(0, radStartX, lifeTimer.GetTimeRate());

	//euler軸へ変換
	enemy->RotVecPlus(aLookat.ToEuler());
	enemy->RotVecPlus({ radX ,0,0});

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

std::string EnemyPreState::GetStateStr()
{
	return "PreAttack";
}

EnemyNowAttackState::EnemyNowAttackState()
{
	isStart = true;
	chargeTimer.Start();
}

void EnemyNowAttackState::Update(Enemy* enemy)
{
	enemy->SetAttackStateStr(EnemyNowAttackState::GetStateStr());

	//初回フレーム時にパーティクル発生
	if (isStart) {
		isStart = false;
	}

	oldChargeTime = chargeTimer.GetTimeRate();
	chargeTimer.Update();
	
	//突撃させる(移動量に足したい)
	//いったん地面に沿うように
	Vector3 nowMove = OutQuadVec3(enemy->attackStartPos, enemy->attackEndPos,chargeTimer.GetTimeRate());
	nowMove.y = 0;
	Vector3 oldMove = OutQuadVec3(enemy->attackStartPos, enemy->attackEndPos, oldChargeTime);
	oldMove.y = 0;

	enemy->MoveVecPlus(nowMove - oldMove);

	//向く方向のベクトルを取得
	Vector3 aVec = enemy->attackEndPos - enemy->attackStartPos;
	aVec.Normalize();
	aVec.y = 0;

	//ターゲットの方向を向いてくれる
	Quaternion aLookat = Quaternion::LookAt(aVec);

	//前30度へ傾く
	float radX = Util::AngleToRadian(30.f);

	//euler軸へ変換
	enemy->RotVecPlus(aLookat.ToEuler());
	enemy->RotVecPlus({ radX ,0,0 });

	if (chargeTimer.GetEnd()) {
		//遷移命令
		enemy->ChangeAttackState<EnemyEndAttackState>();
	}
}

std::string EnemyNowAttackState::GetStateStr()
{
	return "NowAttack";
}

EnemyEndAttackState::EnemyEndAttackState() 
{
	postureTimer.Start();
}

void EnemyEndAttackState::Update(Enemy* enemy)
{
	enemy->SetAttackStateStr(EnemyEndAttackState::GetStateStr());

	postureTimer.Update();

	//姿勢を戻す
	//向く方向のベクトルを取得
	Vector3 aVec = enemy->attackEndPos - enemy->attackStartPos;
	aVec.Normalize();
	aVec.y = 0;

	//ターゲットの方向を向いてくれる
	Quaternion aLookat = Quaternion::LookAt(aVec);

	//前30度へ傾く
	float radStartX = Util::AngleToRadian(30.f);

	//モーション遷移
	float radX = Easing::InQuad(radStartX, 0, postureTimer.GetTimeRate());

	//euler軸へ変換
	enemy->RotVecPlus(aLookat.ToEuler());
	enemy->RotVecPlus({ radX ,0,0 });

	if (postureTimer.GetEnd()) {
		//遷移命令
		//enemy->ChangeAttackState<EnemySeekState>();
		enemy->ChangeAttackState<EnemyBackOriginState>();
	}
}

std::string EnemyEndAttackState::GetStateStr()
{
	return "EndAttack";
}

EnemySeekState::EnemySeekState()
{
	seekTimer.Start();
}

void EnemySeekState::Update(Enemy* enemy)
{
	enemy->SetAttackStateStr(EnemySeekState::GetStateStr());

	//探す タイマーを回す
	seekTimer.Update();
	
	ColPrimitive3D::Sphere sphere;
	sphere.pos = enemy->GetTarget()->mTransform.position;
	sphere.r = enemy->attackHitCollider.r;

	//見つかったら
	if (ColPrimitive3D::CheckSphereToSphere(enemy->collider, sphere))
	{
		//攻撃ステートへ
		enemy->ChangeAttackState<EnemyFindState>();
	}

	//この時間中探しても見つからなかったら
	if (seekTimer.GetEnd()) {
		//戻るステートへ
		enemy->ChangeAttackState<EnemyBackOriginState>();
	}
}

std::string EnemySeekState::GetStateStr()
{
	return "Seek";
}

EnemyBackOriginState::EnemyBackOriginState()
{
	
}

void EnemyBackOriginState::Update(Enemy* enemy)
{
	enemy->SetAttackStateStr(EnemyBackOriginState::GetStateStr());

	Vector3 moveVec = enemy->GetOriginPos() - enemy->GetPos();
	moveVec.Normalize();
	moveVec.y = 0;
	moveVec *= enemy->GetMoveSpeed();
	enemy->MoveVecPlus(moveVec);

	//ターゲットの方向を向いてくれる
	Quaternion aLookat = Quaternion::LookAt(moveVec);
	//euler軸へ変換
	enemy->RotVecPlus(aLookat.ToEuler());

	//その途中でプレイヤーを見つけたらもっかい攻撃へ
	ColPrimitive3D::Sphere sphere;
	sphere.pos = enemy->GetTarget()->mTransform.position;
	sphere.r = enemy->attackHitCollider.r;
	//見つかったら
	if (ColPrimitive3D::CheckSphereToSphere(enemy->collider, sphere))
	{
		//攻撃ステートへ
		enemy->ChangeAttackState<EnemyFindState>();
	}

	//初期位置にたどり着いたら通常モードへ
	ColPrimitive3D::Sphere origin;
	origin.pos = enemy->GetOriginPos();
	origin.r = 2;
	ColPrimitive3D::Sphere now;
	now.pos = enemy->GetPos();
	now.r = 2;

	//適当な当たり判定を作り、当たっているなら終了
	if (ColPrimitive3D::CheckSphereToSphere(origin, now)) {
		//差を埋めて初期位置へ
		enemy->SetPos(enemy->GetOriginPos());
		enemy->BehaviorReset();

		//遷移命令
		enemy->ChangeAttackState<EnemyNormalState>();
	}
}

std::string EnemyBackOriginState::GetStateStr()
{
	return "BackOrigin";
}
