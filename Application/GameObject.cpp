#include "GameObject.h"

GameObject::GameObject() : isAlive(true)
{
}

Vector3 GameObject::GetPos() const
{
	return obj.mTransform.position;
}

Vector3 GameObject::GetScale() const
{
	return obj.mTransform.scale;
}

bool GameObject::GetIsAlive() const
{
	return isAlive;
}

void GameObject::UpdateCollider()
{
	collider.pos = GetPos();

	//もっともデカいスケールを半径とする
	float maxR = GetScale().x;
	if (maxR < GetScale().y)maxR = GetScale().y;
	if (maxR < GetScale().z)maxR = GetScale().z;
	
	collider.r = maxR;
}
