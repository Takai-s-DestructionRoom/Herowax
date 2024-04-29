#include "Tank.h"

Tank::Tank(ModelObj* target_) : Enemy(target_)
{
	obj = PaintableModelObj(Model::Load("./Resources/Model/enemy/enemy.obj", "enemy", true));
}

void Tank::Update()
{
	BaseUpdate();
}
