#pragma once
#include "GameObject.h"
#include "Easing.h"

class EnemyShot : public GameObject
{
public:
	static void LoadResource();

	void Init()override;
	void Update()override;
	void Draw()override;

	//-----ゲッター-----//
	//ダメージ取得
	float GetDamage();

	//-----セッター-----//
	//初期パラメータ設定
	void SetParam(const Vector3& position,const Vector3& moveVec_);
	//ダメージ設定
	void SetDamage(float damage);
	//速さ設定
	void SetMoveSpeed(float moveSpeed_);
	//生存時間設定
	void SetLifeTime(float lifeTime_);

private:
	Vector3 moveVec;
	Vector3 rotVec;

	float rotSpeed = 0.5f;
	float moveSpeed = 0.5f;

	float damage = 1;

	Easing::EaseTimer lifeTimer;
};

