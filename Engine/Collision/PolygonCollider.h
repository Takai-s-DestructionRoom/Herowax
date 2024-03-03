/*
* @file PolygonCollider.h
* @brief ポリゴンで当たり判定をするコライダー
*/

#pragma once
#include "Colliders.h"
#include "ColPrimitive3D.h"

class PolygonCollider : public ICollider {
public:
	//ポリゴン情報、各頂点は時計回りで指定されていること
	ColPrimitive3D::Triangle mPolygon{};

	PolygonCollider() {}
	PolygonCollider(Vector3 posA, Vector3 posB, Vector3 posC) {
		mPolygon.p0 = posA;
		mPolygon.p1 = posB;
		mPolygon.p2 = posC;

		Vector3 v1 = posB - posA;
		Vector3 v2 = posC - posB;

		Vector3 normal = v1.Cross(v2);
		normal.Normalize();
		mPolygon.normal = normal;
	}
	PolygonCollider(Vector3 posA, Vector3 posB, Vector3 posC, Vector3 normal) {
		mPolygon.p0 = posA;
		mPolygon.p1 = posB;
		mPolygon.p2 = posC;
		mPolygon.normal = normal;
	}

	virtual std::string GetTypeIndentifier() override {
		return "PolygonCollider";
	}

	virtual bool Collide(ICollider* hit, CollisionInfo* outputInfo) override;
};
