/*
* @file ICollider.h
* @brief コライダーの基底クラス
*/

#pragma once
#include <functional>
#include <string>
#include "Vector3.h"

class ICollider;

struct CollisionInfo {
	ICollider* hitCollider = nullptr; //衝突した相手
	bool hasClosestPos = false; //最近接点情報を持っているか
	Vector3 closestPos; //最近接点(これ交点で良くね)
	bool hasDistance = false; //距離を持っているか
	float distance = 0; //衝突点への距離
	bool hasInter = false; //交点を持っているか
	Vector3 inter; //交点
};

class Colliders;
class ICollider {
	friend class Colliders;
public:
	virtual ~ICollider() = default;

	virtual std::string GetTypeIndentifier() = 0;
	virtual void Update() {} //必要な子だけ使ってくれよな
	virtual bool Collide(ICollider* hit, CollisionInfo* outputInfo) = 0;

	//衝突した相手がいるかどうかを得る
	bool HasCollision() {
		return mHasCollision;
	}

	bool mIsActive = false;
	std::function<void(CollisionInfo)> mFuncOnCollision;

private:
	bool mHasCollision = false;
};
