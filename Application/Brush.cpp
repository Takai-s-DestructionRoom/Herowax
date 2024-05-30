#include "Brush.h"

void Brush::Init()
{
	sprite = TextureManager::Load("./Resources/Brush.png", "Brush");
}

void Brush::Update()
{
	timer.Update();

	sprite.mTransform.scale = {
		Easing::OutQuad(sizeMin.x,sizeMax.x,timer.GetTimeRate()),
		Easing::OutQuad(sizeMin.y,sizeMax.y,timer.GetTimeRate()),
		0
	};

	sprite.mTransform.UpdateMatrix();
	sprite.TransferBuffer();
}

void Brush::Draw()
{
	sprite.Draw();
}

void Brush::Start(Vector2 sizeMin_, Vector2 sizeMax_)
{
	sizeMin = sizeMin_;
	sizeMax = sizeMax_;
	timer.Start();

	sprite.mTransform.position = {
		Util::GetRand(0.f, (float)Util::WIN_WIDTH),
		Util::GetRand(0.f, (float)Util::WIN_HEIGHT),
		0
	};

	sprite.mMaterial.mColor = {
		Util::GetRand(0.0f,1.0f),
		Util::GetRand(0.0f,1.0f),
		Util::GetRand(0.0f,1.0f),
		1.0f
	};
}
