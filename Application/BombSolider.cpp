#include "BombSolider.h"

BombSolider::BombSolider(ModelObj* target_) : Enemy(target_)
{
	obj = PaintableModelObj(Model::Load("./Resources/Model/bombSolider/bombSolider.obj", "bombSolider", true));
	obj.SetupPaint();
}
