#include "Cube.h"
#include "Vector3.h"

Cube::Cube(TextureHandle texture, Vector2 size, bool forceSize)
{
	mSize = size;
	for (int32_t i = 0; i < 6; i++) {
		mFaces[i].SetParent(this);
		mFaces[i].SetTexture(texture);
	}

	for (int32_t i = 0; i < 4; i++) {
		mFaces[i].SetSize(size, forceSize);
	}

	Vector2 fSize = mFaces[Direction::Front].GetSize();

	for (int32_t i = 4; i < 6; i++) {
		mFaces[i].SetSize({ fSize.x, fSize.x }, true);
	}

	for (int32_t i = 0; i < 6; i++) {
		mFaces[i].SetParent(this);
		mFaces[i].Init();
	}

	//雑にキューブにするよ
	mFaces[Direction::Front].mTransform.position = { 0, 0, -fSize.x / 2.0f };
	mFaces[Direction::Right].mTransform.position = { -fSize.x / 2.0f, 0, 0 };
	mFaces[Direction::Right].mTransform.rotation.y = Util::AngleToRadian(90);
	mFaces[Direction::Back].mTransform.position = { 0, 0, fSize.x / 2.0f };
	mFaces[Direction::Back].mTransform.rotation.y = Util::AngleToRadian(180);
	mFaces[Direction::Left].mTransform.position = { fSize.x / 2.0f, 0, 0 };
	mFaces[Direction::Left].mTransform.rotation.y = Util::AngleToRadian(270);
	mFaces[Direction::Top].mTransform.position = { 0, fSize.y / 2.0f, 0 };
	mFaces[Direction::Top].mTransform.rotation = { Util::AngleToRadian(90), Util::AngleToRadian(180), 0};
	mFaces[Direction::Bottom].mTransform.position = { 0, -fSize.y / 2.0f, 0 };
	mFaces[Direction::Bottom].mTransform.rotation = { Util::AngleToRadian(-90), 0, 0 };
}

void Cube::SetTexture(TextureHandle texture, Direction direction)
{
	mFaces[direction].SetTexture(texture);
	mFaces[direction].Init();
}

void Cube::SetAllTexture(TextureHandle texture)
{
	for (int32_t i = 0; i < 6; i++) {
		mFaces[i].SetTexture(texture);
		mFaces[i].Init();
	}
}

void Cube::SetAllAlpha(float alpha)
{
	for (int32_t i = 0; i < 6; i++) {
		mFaces[i].mMaterial.mColor.a = alpha;
	}
}

void Cube::UpdateFaces()
{
	for (int32_t i = 0; i < 6; i++) {
		mFaces[i].SetParent(this);
		mFaces[i].mTransform.UpdateMatrix();
	}
}

void Cube::TransferBuffer(ViewProjection viewprojection)
{
	for (int32_t i = 0; i < 6; i++) {
		mFaces[i].TransferBuffer(viewprojection);
	}
}

void Cube::Draw()
{
	for (int32_t i = 0; i < 6; i++) {
		mFaces[i].Draw();
	}
}

void Cube::DrawCommands()
{
	for (int32_t i = 0; i < 6; i++) {
		mFaces[i].DrawCommands();
	}
}
