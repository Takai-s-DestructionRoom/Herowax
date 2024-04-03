/*
* @file PlaneCollider.h
* @brief 球で当たり判定をするコライダー
*/

#pragma once
#include "Colliders.h"
#include "ColPrimitive3D.h"

class PlaneCollider : public ICollider {
public:
	ColPrimitive3D::Plane mPlane{};

	PlaneCollider() {}
	PlaneCollider(Vector3 normal, float dist) {
		mPlane.normal = normal;
		mPlane.distance = dist;
	}
	virtual std::string GetTypeIndentifier() override {
		return "PlaneCollider";
	}

	virtual bool Collide(ICollider* hit, CollisionInfo* outputInfo) override;
};
