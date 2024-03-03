#include "EObjTapNote.h"
#include "EditorScene.h"
#include <SimpleDrawer.h>
#include "ColPrimitive2D.h"
#include "EditorAction.h"

EObjTapNote::EObjTapNote(EditorScene* scene)
	: IEditorObject(scene)
{}

std::unique_ptr<IEditorObject> EObjTapNote::Clone() const
{
	return std::make_unique<EObjTapNote>(*this);
}

std::unique_ptr<EditorAction> EObjTapNote::GetSavePoint()
{
	std::unique_ptr<EditorAction> ptr = std::make_unique<EAModifyEOField<EObjTapNote>>(mUuid, 0, *this);
	return ptr;
}

Vector2 EObjTapNote::GetPos()
{
	float x1 = 1180 + 100 / 4.0f * mLane;
	float x2 = x1 + 100 / 4.0f;
	return Vector2((x1 + x2) / 2.0f, mScene->GetOriginScreenPosition(mBeat));
}

Beat EObjTapNote::GetBeat()
{
	return mBeat;
}

bool EObjTapNote::Collide(float x, float y)
{
	int32_t laneSize = mScene->sMaxLaneX - mScene->sMinLaneX;

	float noteX1 = mScene->sMinLaneX + laneSize / 4.0f * mLane;
	float noteX2 = noteX1 + laneSize / 4.0f;

	float noteY = mScene->GetScreenPosition(mBeat);

	ColPrimitive2D::Point point{ Vector2(x, y) };
	ColPrimitive2D::Rect rect(Vector2(noteX1, noteY), Vector2(noteX2, noteY - 15));

	return ColPrimitive2D::IsHit(point, rect);
}

bool EObjTapNote::Collide(float x1, float y1, float x2, float y2)
{
	int32_t laneSize = mScene->sMaxLaneX - mScene->sMinLaneX;

	float noteX1 = mScene->sMinLaneX + laneSize / 4.0f * mLane;
	float noteX2 = noteX1 + laneSize / 4.0f;

	float noteY = mScene->GetScreenPosition(mBeat);

	ColPrimitive2D::Rect rectA(Vector2(x1, y1), Vector2(x2, y2));
	ColPrimitive2D::Rect rectB(Vector2(noteX1, noteY), Vector2(noteX2, noteY - 15));

	return ColPrimitive2D::IsHit(rectA, rectB);
}

bool EObjTapNote::IsSelected()
{
	return mIsSelected;
}

int32_t EObjTapNote::GetSelectCountInThisInstance()
{
	return mIsSelected ? 1 : 0;
}

void EObjTapNote::Select(float, float, bool keep)
{
	if (keep && mIsSelected) return;
	mIsSelected = !mIsSelected;
}

void EObjTapNote::Select(float, float, float, float, bool keep)
{
	if (keep && mIsSelected) return;
	mIsSelected = !mIsSelected;
}

void EObjTapNote::UnSelect()
{
	mIsSelected = false;
}

void EObjTapNote::MoveStart()
{
	int32_t laneSize = EditorScene::sMaxLaneX - EditorScene::sMinLaneX;

	{
		int32_t x1 = EditorScene::sMinLaneX + laneSize / 4 * mLane;
		int32_t x2 = EditorScene::sMinLaneX + laneSize / 4 * (mLane + 1);

		mMoveOrigin.x = (x1 + x2) / 2.0f;
	}
	mMoveOrigin.y = mScene->GetScreenPosition(mBeat);
}

void EObjTapNote::Move(float x, float y)
{
	//相対仮想カーソルの座標
	Vector2 mouse = mMoveOrigin;

	int32_t laneSize = EditorScene::sMaxLaneX - EditorScene::sMinLaneX;

	mouse.x += x;
	mouse.y += y;

	if (mouse.x < EditorScene::sMinLaneX) {
		mLane = 0;
	}
	else if (mouse.x > EditorScene::sMaxLaneX) {
		mLane = 3;
	}
	else {
		for (int i = 0; i <= 3; i++) {
			int32_t x1 = EditorScene::sMinLaneX + laneSize / 4 * i;
			int32_t x2 = EditorScene::sMinLaneX + laneSize / 4 * (i + 1);

			if (mouse.x >= x1 && mouse.x <= x2) {
				mLane = i;
				break;
			}
		}
	}

	mBeat = mScene->GetVirtualCursorBeat(mouse.y);
}

void EObjTapNote::Draw()
{
	int32_t laneSize = mScene->sMaxLaneX - mScene->sMinLaneX;

	float x1 = mScene->sMinLaneX + laneSize / 4.0f * mLane;
	float x2 = x1 + laneSize / 4.0f;

	float y = mScene->GetScreenPosition(mBeat);

	if (y < 0 || y > 720) {
		return;
	}

	SimpleDrawer::DrawBox(x1 + 4, y - 15, x2 - 4, y, 5, Color(0, 1, 1, 1), true);
	if(mIsSelected) SimpleDrawer::DrawBox(x1 + 4, y - 15, x2 - 4, y, 5, Color(1, 0, 0, 1), false, 2);
}

void EObjTapNote::DrawToMiniMap()
{
	float x1 = 1180 + 100 / 4.0f * mLane;
	float x2 = x1 + 100 / 4.0f;

	float y = mScene->GetOriginScreenPosition(mBeat);

	float multipiler = 720 / mScene->GetAllScreenLength();
	float posY1 = 720 - y * multipiler - 15 * multipiler;
	float posY2 = 720 - y * multipiler;
	if (posY2 - posY1 <= 1.0f) posY2 = posY1 + 1;
	SimpleDrawer::DrawBox(x1, posY1, x2, posY2, 100, 0x00ffff, true);
}

void EObjTapNote::TempDraw(EditorScene* scene, int32_t lane, Beat beat)
{
	int32_t laneSize = EditorScene::sMaxLaneX - EditorScene::sMinLaneX;

	float x1 = EditorScene::sMinLaneX + laneSize / 4.0f * lane;
	float x2 = x1 + laneSize / 4.0f;

	float y = scene->GetScreenPosition(beat);

	if (y < 0 || y > 720) {
		return;
	}

	SimpleDrawer::DrawBox(x1 + 4, y - 15, x2 - 4, y, 5, Color(0, 1, 1, 0.3f), true);
}
