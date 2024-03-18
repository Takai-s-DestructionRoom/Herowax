#include "Terrain.h"
#include "Camera.h"

Terrain::Terrain() : GameObject()
{
	classname = __func__;
}

void Terrain::Init()
{
	
}

void Terrain::Update()
{
	UpdateCollider();

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Terrain::Draw()
{
	if (isAlive)
	{
		obj.Draw();
		DrawCollider();
	}
}
