#pragma once
#include "Easing.h"

class Player;

//主に攻撃周りを分離しやすくするためのステート
class PlayerState
{
public:
	virtual void Update(Player* player) = 0;
	virtual ~PlayerState() {};
};

class PlayerNormal : public PlayerState
{
public:
	void Update(Player* player);
};

class PlayerPablo : public PlayerState
{
public:
	void Update(Player* player);
private:
	float oldLength = 0;
};