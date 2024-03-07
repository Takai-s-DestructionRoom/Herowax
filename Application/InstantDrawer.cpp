#include "InstantDrawer.h"
#include <minmax.h>

void InstantDrawer::DrawInit()
{
	for (auto& instant : Get()->sSprites)
	{
		if (instant.isUse)
		{
			instant.isUse = false;
			instant.mMaterial.mColor = { 1,1,1,1 };
			instant.mTransform.position = { -100,-100,-100 };
			instant.mTransform.scale = { 0,0,0 };
			instant.mTransform.rotation = { 0,0,0 };
		}
	}
}

void InstantDrawer::DrawGraph(const float& x, const float& y, 
	float sizerateX, float sizerateY, float angle,
	const std::string& handle, const Anchor& anchor)
{
	//ハンドルが空なら描画をキャンセル
	if (handle == "")return;

	for (auto& instant : Get()->sSprites)
	{
		if (!instant.isUse)
		{
			instant.isUse = true;
			instant.SetTexture(handle);
			instant.mTransform.position = { x,y,0 };
			instant.mTransform.rotation = { 0,0,Util::AngleToRadian(angle) };
			instant.mTransform.scale = { sizerateX,sizerateY,0 };
			switch (anchor)
			{
			case InstantDrawer::Anchor::LEFT:
				instant.SetAnchor({ 0.f, 0.5f });
				break;
			case InstantDrawer::Anchor::CENTER:
				instant.SetAnchor({ 0.5f,0.5f });
				break;
			case InstantDrawer::Anchor::RIGHT:
				instant.SetAnchor({ 1.0f,0.5f });
				break;
			case InstantDrawer::Anchor::UP:
				instant.SetAnchor({ 0.5f,0.f });
				break;
			case InstantDrawer::Anchor::DOWN:
				instant.SetAnchor({ 0.5f,1.0f });
				break;
			}
			break;
		}
	}
}

void InstantDrawer::AllUpdate()
{
	for (auto& sprite : Get()->sSprites)
	{
		if (sprite.isUse)
		{
			sprite.mTransform.UpdateMatrix();
			sprite.TransferBuffer();
		}
	}
}

void InstantDrawer::AllDraw2D()
{
	std::list<InstantSprite>* hoge = &Get()->sSprites;

	for (auto &sprite : *hoge)
	{
		if (sprite.isUse)
		{
			sprite.Draw();
		}
	}
}

void InstantDrawer::PreCreate()
{
	for (int32_t i = 0; i < SP_MAX; i++)
	{
		Get()->sSprites.emplace_back();
	}
}
