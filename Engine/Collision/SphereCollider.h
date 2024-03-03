/*
* @file SphereCollider.h
* @brief 球で当たり判定をするコライダー
*/

#pragma once
#include "Colliders.h"
#include "ColPrimitive3D.h"

class SphereCollider : public ICollider {
public:
	ColPrimitive3D::Sphere mSphere{};

	SphereCollider() {}
	SphereCollider(Vector3 pos, float r) {
		mSphere.pos = pos;
		mSphere.r = r;
	}
	virtual std::string GetTypeIndentifier() override {
		return "SphereCollider";
	}

	virtual bool Collide(ICollider* hit, CollisionInfo* outputInfo) override;
};
