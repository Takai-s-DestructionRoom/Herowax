#include "Brush.h"

void Brush::Init()
{
	sprite = TextureManager::Load("./Resources/Brush.png", "Brush");
}

void Brush::Update()
{
	openTimer.Update();
	closeTimer.Update();

	if (closeTimer.GetStarted())
	{
		sprite.mTransform.scale = {
			Easing::OutBack(sizeMin.x,sizeMax.x,closeTimer.GetTimeRate()),
			Easing::OutBack(sizeMin.y,sizeMax.y,closeTimer.GetTimeRate()),
			0
		};
	}
	if (openTimer.GetStarted())
	{
		sprite.mTransform.scale = {
			Easing::InBack(sizeMax.x,sizeMin.x,openTimer.GetTimeRate()),
			Easing::InBack(sizeMax.y,sizeMin.y,openTimer.GetTimeRate()),
			0
		};
	}

	sprite.mTransform.UpdateMatrix();
	sprite.TransferBuffer();
}

void Brush::Draw()
{
	sprite.Draw();
}

void Brush::Close(Vector2 pos,Vector2 sizeMin_, Vector2 sizeMax_)
{
	sizeMin = sizeMin_;
	sizeMax = sizeMax_;
	closeTimer.Start();
	openTimer.Reset();

	sprite.mTransform.position.x = pos.x;
	sprite.mTransform.position.y = pos.y;
}

void Brush::Open(Vector2 pos, Vector2 sizeMin_, Vector2 sizeMax_)
{
	sizeMin = sizeMin_;
	sizeMax = sizeMax_;
	openTimer.Start();
	closeTimer.Reset();

	sprite.mTransform.position.x = pos.x;
	sprite.mTransform.position.y = pos.y;
}

bool Brush::GetOpenEnd()
{
	return openTimer.GetEnd();
}

bool Brush::GetCloseEnd()
{
	return closeTimer.GetEnd();
}
