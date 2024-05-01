#pragma once
#include "Enemy.h"

class Tank : public Enemy
{
public:
	Tank(ModelObj* target_);

	//一応updateをラッピングして色々書けるように
	void Update()override;

	static std::string GetEnemyTag();
};

