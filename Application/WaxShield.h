#pragma once
#include "GameObject.h"
#include "Easing.h"

class WaxShield : public GameObject
{
public:
	void Init()override;
	void Update()override;
	void Draw()override;

	//当たったときに呼ぶ処理
	void Hit(int32_t damage);

	//固まっているかを取得
	bool IsSolid();

	//強制的に破壊
	void Break();

	bool GetHitCollider(ColPrimitive3D::Sphere hit);

	int32_t waxSolidCount = 0;
	int32_t requireWaxSolidCount = 60;

private:
	bool oldIsSolid = false;
	Easing::EaseTimer brightTimer = 0.5f;
};

