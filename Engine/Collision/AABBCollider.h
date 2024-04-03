/*
* @file AABBCollider.h
* @brief 球で当たり判定をするコライダー
*/

#pragma once
#include "Colliders.h"
#include "ColPrimitive3D.h"

class AABBCollider : public ICollider {
public:
	ColPrimitive3D::AABB mAABB{};

	AABBCollider() {}
	AABBCollider(Vector3 pos, Vector3 size) {
		mAABB.pos = pos;
		mAABB.size = size;
	}
	virtual std::string GetTypeIndentifier() override {
		return "AABBCollider";
	}

	virtual bool Collide(ICollider* hit, CollisionInfo* outputInfo) override;
};
