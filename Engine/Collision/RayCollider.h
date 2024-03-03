/*
* @file RayCollider.h
* @brief レイで当たり判定をするコライダー
*/

#pragma once
#include "Colliders.h"
#include "ColPrimitive3D.h"

class RayCollider : public ICollider {
public:
	ColPrimitive3D::Ray mRay{};
	bool mOptPierce = false;

	RayCollider() {}
	RayCollider(Vector3 start, Vector3 dir) {
		mRay.start = start;
		mRay.dir = dir;
	}
	RayCollider(Vector3 start, Vector3 dir, bool pierce) {
		mRay.start = start;
		mRay.dir = dir;
		mOptPierce = pierce;
	}
	virtual std::string GetTypeIndentifier() override {
		return "RayCollider";
	}

	virtual void Update() override;
	virtual bool Collide(ICollider* hit, CollisionInfo* outputInfo) override;

private:
	bool mFlagSkip = false;
	float mMemDis = 0;
	CollisionInfo mMemInfo;
	ICollider* mNearest = nullptr;
};
