#pragma once
#include "Vector2.h"
#include "Color.h"
#include "ColPrimitive3D.h"

class PhysicsCircle {
public:
	Vector2 pos;//重心位置
	Vector2 v;//速度
	float theta;//回転角度
	float omega;//角速度
	float M;//質量
	float I;//慣性モーメント
	float r;//半径

	float gravity = 9.8f;//重力

	ColPrimitive3D::Sphere sphere;

public:
	PhysicsCircle() = default;
	PhysicsCircle(const Vector2& center, float radius) {
		pos = center;
		r = radius;
		v = {};
		theta = 0;
		omega = 0;
		M = 1;
		I = 0.5f * M * r * r;
	}

	static void LoadResource();
	void Update();
	void Draw();


	//重心を基準とした相対座標から力積を加える。
	void addImpulseLocal(const Vector2& impulse, const Vector2& addLocalPos);
	//絶対座標から力積を加える。
	void addImpulse(const Vector2& impulse, const Vector2& addPos);
};

