#include "InstantDrawer.h"
#include "Camera.h"
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
	for (auto& instant : Get()->sBills)
	{
		if (instant.isUse)
		{
			instant.isUse = false;
			instant.mTransform.position = { -100,-100,-100 };
		}
	}
}

void InstantDrawer::DrawGraph(const float& x, const float& y,
	float sizerateX, float sizerateY, float angle,
	const std::string& handle, const Color& color,
	const Anchor& anchor/*,
	const float& rectX, const float& rectY,
	const float& rectWidth, const float& rectHeight*/)
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
			instant.mMaterial.mColor = color;
			/*instant.SetTexRect(
				int32_t(rectX),
				int32_t(rectHeight),
				int32_t(rectWidth),
				int32_t(rectHeight));*/
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

void InstantDrawer::DrawGraph3D(const Vector3& pos, float width, float height,
	const std::string& handle, const Color& color)
{
	//ハンドルが空なら描画をキャンセル
	if (handle == "")return;

	for (auto& instant : Get()->sBills)
	{
		if (!instant.isUse)
		{
			instant.isUse = true;
			instant.mUseBillboardY = true;
			//instant.Init(handle,Vector2(width,height));
			instant.mImage.SetTexture(handle);
			instant.mImage.SetSize({ width, height });
			instant.mTransform.position = pos;
			instant.mImage.mMaterial.mColor = color;
			instant.mImage.mBlendMode = Image3D::BlendMode::TransparentAlpha;
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
	for (auto& bill : Get()->sBills)
	{
		if (bill.isUse)
		{
			bill.Update(Camera::sNowCamera->mViewProjection);
		}
	}
}

void InstantDrawer::AllDraw2D()
{
	std::list<InstantSprite>* hoge = &Get()->sSprites;

	for (auto& sprite : *hoge)
	{
		if (sprite.isUse)
		{
			sprite.Draw();
		}
	}
	for (auto& bill : Get()->sBills)
	{
		if (bill.isUse)
		{
			bill.Draw();
		}
	}
}

void InstantDrawer::PreCreate()
{
	if (Get()->mCreated) return;
	Get()->mCreated = true;
	for (int32_t i = 0; i < SP_MAX; i++)
	{
		Get()->sSprites.emplace_back();
	}
	for (int32_t i = 0; i < BILL_MAX; i++)
	{
		Get()->sBills.emplace_back();
	}
}
