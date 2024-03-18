#include "Ground.h"
#include "Camera.h"

Ground::Ground()
{
	classname = __func__;
	obj = ModelManager::Get(Model::Load("./Resources/Model/Cube.obj", "Cube"));
}

void Ground::Init()
{
}

void Ground::Update()
{
	UpdateCollider();

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Ground::Draw()
{
	if (isAlive)
	{
		obj.Draw();
		DrawCollider();
	}
}
