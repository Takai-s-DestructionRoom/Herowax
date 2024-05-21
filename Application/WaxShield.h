#pragma once
#include "GameObject.h"

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

	int32_t waxSolidCount = 0;
	int32_t requireWaxSolidCount = 60;
};

