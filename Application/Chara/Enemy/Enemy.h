#pragma once
#include "../../Chara/Charactor/Charactor.h"

class Enemy : public Charactor
{
public:
	//追跡する対象(プレイヤーを入れる)
	ModelObj* target = nullptr;

public:
	Enemy(ModelObj* target_);
	void Init() override;
	void Update() override;
	void Draw() override;
};

