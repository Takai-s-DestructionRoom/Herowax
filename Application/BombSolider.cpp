#include "BombSolider.h"

BombSolider::BombSolider(ModelObj* target_) : Enemy(target_)
{
	obj = ModelObj(Model::Load("./Resources/Model/bombSolider/bombSolider.obj", "bombSolider", true));
}
