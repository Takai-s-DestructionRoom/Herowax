#pragma once
#include "Enemy.h"
#include "WaxShield.h"

class Tank : public Enemy
{
public:
	Tank(ModelObj* target_);

	//一応updateをラッピングして色々書けるように
	void Update()override;
	void Draw()override;

	static std::string GetEnemyTag();
	//
	WaxShield* GetShield() { return &shield; };

private:
	WaxShield shield;
};

