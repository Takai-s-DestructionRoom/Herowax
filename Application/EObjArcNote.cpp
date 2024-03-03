#include "EObjArcNote.h"
#include "EditorScene.h"
#include "Vector3.h"
#include <SimpleDrawer.h>
#include <ColPrimitive2D.h>
#include "EditorAction.h"

EObjArcNote::EObjArcNote(EditorScene* scene)
	: IEditorObject(scene)
{
}

std::unique_ptr<IEditorObject> EObjArcNote::Clone() const
{
	return std::make_unique<EObjArcNote>(*this);
}

std::unique_ptr<EditorAction> EObjArcNote::GetSavePoint()
{
	std::unique_ptr<EditorAction> ptr = std::make_unique<EAModifyEOField<EObjArcNote>>(mUuid, 0, *this);
	return ptr;
}

Vector2 EObjArcNote::GetPos()
{
	return Vector2(mStartPos.x, mScene->GetOriginScreenPosition(mStartBeat));
}

Beat EObjArcNote::GetBeat()
{
	return mStartBeat;
}

bool EObjArcNote::Collide(float x, float y)
{
	ColPrimitive2D::Point point{ {x, y} };
	int32_t laneSize = mScene->sMaxLaneX - mScene->sMinLaneX;
	int32_t laneCenter = (mScene->sMinLaneX + mScene->sMaxLaneX) / 2;
	int32_t arcHeightSize = mScene->sMaxArcHeightX - mScene->sMinArcHeightX;

	//始点
	float startY = mScene->GetScreenPosition(mStartBeat);

	float startX = laneCenter + (mStartPos.x / 8.0f) * (laneSize / 2);
	float startX2 = mScene->sMaxArcHeightX - (((mStartPos.y - 1) / 4.0f) * arcHeightSize);

	ColPrimitive2D::Rect startRect({ startX - 10, startY - 10 }, { startX + 10, startY + 10 });
	ColPrimitive2D::Rect startRect2({ startX2 - 10, startY - 10 }, { startX2 + 10, startY + 10 });
	if (ColPrimitive2D::IsHit(point, startRect) || ColPrimitive2D::IsHit(point, startRect2)) {
		return true;
	}

	//終点
	float endY = mScene->GetScreenPosition(mEndBeat);

	float endX = laneCenter + (mEndPos.x / 8.0f) * (laneSize / 2);
	float endX2 = mScene->sMaxArcHeightX - (((mEndPos.y - 1) / 4.0f) * arcHeightSize);

	ColPrimitive2D::Rect endRect({ endX - 12, endY - 12 }, { endX + 12, endY + 12 });
	ColPrimitive2D::Rect endRect2({ endX2 - 12, endY - 12 }, { endX2 + 12, endY + 12 });
	if (ColPrimitive2D::IsHit(point, endRect) || ColPrimitive2D::IsHit(point, endRect2)) {
		return true;
	}

	//制御点
	for (auto& cp : mControlPoints) {
		float cpY = mScene->GetScreenPosition(cp.beat);

		float cpX = laneCenter + (cp.pos.x / 8.0f) * (laneSize / 2);
		float cpX2 = mScene->sMaxArcHeightX - (((cp.pos.y - 1) / 4.0f) * arcHeightSize);

		ColPrimitive2D::Rect cpRect({ cpX - 12, cpY - 12 }, { cpX + 12, cpY + 12 });
		ColPrimitive2D::Rect cpRect2({ cpX2 - 12, cpY - 12 }, { cpX2 + 12, cpY + 12 });
		if (ColPrimitive2D::IsHit(point, cpRect) || ColPrimitive2D::IsHit(point, cpRect2)) {
			return true;
		}
	}

	//線分
	{
		Vector2 prevPos = { startX, startY };
		Vector2 startPos;
		Vector2 endPos;

		auto itr = mControlPoints.begin();

		while (true) {
			startPos = prevPos;
			if (itr != mControlPoints.end()) {
				float cpY = mScene->GetScreenPosition(itr->beat);
				float cpX = laneCenter + (itr->pos.x / 8.0f) * (laneSize / 2);
				endPos = { cpX, cpY };
			}
			else {
				endPos = { endX, endY };
			}

			if ((startPos.y < 0 && endPos.y < 0) || (startPos.y > 720 && endPos.y > 720)) {
				if (itr != mControlPoints.end()) {
					float cpY = mScene->GetScreenPosition(itr->beat);
					float cpX = laneCenter + (itr->pos.x / 8.0f) * (laneSize / 2);
					prevPos = { cpX, cpY };
					itr++;
					continue;
				}
				else {
					break;
				}
			}

			ColPrimitive2D::Capsule capsule(startPos, endPos, 5);
			if (ColPrimitive2D::IsHit(point, capsule)) {
				return true;
			}

			if (itr != mControlPoints.end()) {
				float cpY = mScene->GetScreenPosition(itr->beat);
				float cpX = laneCenter + (itr->pos.x / 8.0f) * (laneSize / 2);
				prevPos = { cpX, cpY };
				itr++;
			}
			else {
				break;
			}
		}
	}

	return false;
}

bool EObjArcNote::Collide(float x1, float y1, float x2, float y2) {
	ColPrimitive2D::Rect rect({x1, y1} , {x2, y2});
	int32_t laneSize = mScene->sMaxLaneX - mScene->sMinLaneX;
	int32_t laneCenter = (mScene->sMinLaneX + mScene->sMaxLaneX) / 2;
	int32_t arcHeightSize = mScene->sMaxArcHeightX - mScene->sMinArcHeightX;

	//始点
	float startY = mScene->GetScreenPosition(mStartBeat);

	float startX = laneCenter + (mStartPos.x / 8.0f) * (laneSize / 2);
	float startX2 = mScene->sMaxArcHeightX - (((mStartPos.y - 1) / 4.0f) * arcHeightSize);

	ColPrimitive2D::Rect startRect({ startX - 10, startY - 10 }, { startX + 10, startY + 10 });
	ColPrimitive2D::Rect startRect2({ startX2 - 10, startY - 10 }, { startX2 + 10, startY + 10 });
	if (ColPrimitive2D::IsHit(rect, startRect) || ColPrimitive2D::IsHit(rect, startRect2)) {
		return true;
	}

	//終点
	float endY = mScene->GetScreenPosition(mEndBeat);

	float endX = laneCenter + (mEndPos.x / 8.0f) * (laneSize / 2);
	float endX2 = mScene->sMaxArcHeightX - (((mEndPos.y - 1) / 4.0f) * arcHeightSize);

	ColPrimitive2D::Rect endRect({ endX - 12, endY - 12 }, { endX + 12, endY + 12 });
	ColPrimitive2D::Rect endRect2({ endX2 - 12, endY - 12 }, { endX2 + 12, endY + 12 });
	if (ColPrimitive2D::IsHit(rect, endRect) || ColPrimitive2D::IsHit(rect, endRect2)) {
		return true;
	}

	//制御点
	for (auto& cp : mControlPoints) {
		float cpY = mScene->GetScreenPosition(cp.beat);

		float cpX = laneCenter + (cp.pos.x / 8.0f) * (laneSize / 2);
		float cpX2 = mScene->sMaxArcHeightX - (((cp.pos.y - 1) / 4.0f) * arcHeightSize);

		ColPrimitive2D::Rect cpRect({ cpX - 12, cpY - 12 }, { cpX + 12, cpY + 12 });
		ColPrimitive2D::Rect cpRect2({ cpX2 - 12, cpY - 12 }, { cpX2 + 12, cpY + 12 });
		if (ColPrimitive2D::IsHit(rect, cpRect) || ColPrimitive2D::IsHit(rect, cpRect2)) {
			return true;
		}
	}
	return false;
}

bool EObjArcNote::IsSelected()
{
	for (auto& cp : mControlPoints) {
		if (cp.select) {
			return true;
		}
	}
	return  mSelectStartPos || mSelectEndPos;
}

int32_t EObjArcNote::GetSelectCountInThisInstance()
{
	int32_t count = 0;
	if (mSelectStartPos) count++;
	if (mSelectEndPos) count++;
	for (auto& cp : mControlPoints) {
		if (cp.select) count++;
	}
	return count;
}

void EObjArcNote::Select(float x, float y, bool keep)
{
	ColPrimitive2D::Point point{ {x, y} };
	int32_t laneSize = mScene->sMaxLaneX - mScene->sMinLaneX;
	int32_t laneCenter = (mScene->sMinLaneX + mScene->sMaxLaneX) / 2;
	int32_t arcHeightSize = mScene->sMaxArcHeightX - mScene->sMinArcHeightX;


	//始点
	float startY = mScene->GetScreenPosition(mStartBeat);

	float startX = laneCenter + (mStartPos.x / 8.0f) * (laneSize / 2);
	float startX2 = mScene->sMaxArcHeightX - (((mStartPos.y - 1) / 4.0f) * arcHeightSize);

	ColPrimitive2D::Rect startRect({ startX - 10, startY - 10 }, { startX + 10, startY + 10 });
	ColPrimitive2D::Rect startRect2({ startX2 - 10, startY - 10 }, { startX2 + 10, startY + 10 });
	if (ColPrimitive2D::IsHit(point, startRect) || ColPrimitive2D::IsHit(point, startRect2)) {
		if (keep) {
			mSelectStartPos = true;
		}
		else {
			mSelectStartPos = !mSelectStartPos;
		}
		mSelectStartHeight = ColPrimitive2D::IsHit(point, startRect2);
	}

	//終点
	float endY = mScene->GetScreenPosition(mEndBeat);

	float endX = laneCenter + (mEndPos.x / 8.0f) * (laneSize / 2);
	float endX2 = mScene->sMaxArcHeightX - (((mEndPos.y - 1) / 4.0f) * arcHeightSize);

	ColPrimitive2D::Rect endRect({ endX - 12, endY - 12 }, { endX + 12, endY + 12 });
	ColPrimitive2D::Rect endRect2({ endX2 - 12, endY - 12 }, { endX2 + 12, endY + 12 });
	if (ColPrimitive2D::IsHit(point, endRect) || ColPrimitive2D::IsHit(point, endRect2)) {
		if (keep) {
			mSelectEndPos = true;
		}
		else {
			mSelectEndPos = !mSelectEndPos;
		}
		mSelectEndHeight = ColPrimitive2D::IsHit(point, endRect2);
	}

	//制御点
	for (auto& cp : mControlPoints) {
		float cpY = mScene->GetScreenPosition(cp.beat);

		float cpX = laneCenter + (cp.pos.x / 8.0f) * (laneSize / 2);
		float cpX2 = mScene->sMaxArcHeightX - (((cp.pos.y - 1) / 4.0f) * arcHeightSize);

		ColPrimitive2D::Rect cpRect({ cpX - 12, cpY - 12 }, { cpX + 12, cpY + 12 });
		ColPrimitive2D::Rect cpRect2({ cpX2 - 12, cpY - 12 }, { cpX2 + 12, cpY + 12 });
		if (ColPrimitive2D::IsHit(point, cpRect) || ColPrimitive2D::IsHit(point, cpRect2)) {
			if (keep) {
				cp.select = true;
			}
			else {
				cp.select = !cp.select;
			}
			cp.selectHeight = ColPrimitive2D::IsHit(point, cpRect2);
		}
	}

	//線分(分割処理含むぞ)
	{
		Vector2 prevPos = { startX, startY };
		Vector2 startPos;
		Vector2 endPos;

		auto itr = mControlPoints.begin();

		while (true) {
			startPos = prevPos;
			if (itr != mControlPoints.end()) {
				float cpY = mScene->GetScreenPosition(itr->beat);
				float cpX = laneCenter + (itr->pos.x / 8.0f) * (laneSize / 2);
				endPos = { cpX, cpY };
			}
			else {
				endPos = { endX, endY };
			}

			if ((startPos.y < 0 && endPos.y < 0) || (startPos.y > 720 && endPos.y > 720)) {
				if (itr != mControlPoints.end()) {
					float cpY = mScene->GetScreenPosition(itr->beat);
					float cpX = laneCenter + (itr->pos.x / 8.0f) * (laneSize / 2);
					prevPos = { cpX, cpY };
					itr++;
					continue;
				}
				else {
					break;
				}
			}

			ColPrimitive2D::Capsule capsule(startPos, endPos, 5);
			ColPrimitive2D::ColResult result = ColPrimitive2D::CheckHit(point, capsule);
			if (result.hit) {
				if (!((startPos - result.onLinePos).Length() <= 20 || (endPos - result.onLinePos).Length() <= 20)) {
					EObjArcNote::ControlPoint cp;
					cp.beat = mScene->GetVirtualCursorBeat(result.onLinePos.y);
					if(itr != mControlPoints.end()) cp.pos.y = itr->pos.y;
					else cp.pos.y = 1;

					float nearX = 1;
					float nearDistance = abs(EditorScene::sMinLaneX - result.onLinePos.x);
					for (float tx = -8; tx <= 8; tx += 0.25f) {
						float sx = laneCenter + (tx / 8.0f) * (laneSize / 2);
						float distance = abs(sx - result.onLinePos.x);
						if (distance <= nearDistance) {
							nearX = tx;
							nearDistance = distance;
						}
					}
					cp.pos.x = nearX;
					cp.select = true;

					mScene->AddEditorAction(std::make_unique<EAModifyEOField<EObjArcNote>>(mUuid, 0, *this));
					mControlPoints.push_back(cp);
					std::stable_sort(mControlPoints.begin(), mControlPoints.end(), [](EObjArcNote::ControlPoint const& lhs, EObjArcNote::ControlPoint const& rhs) {
						return lhs.beat < rhs.beat;
					});
					break;
				}
			}

			if (itr != mControlPoints.end()) {
				float cpY = mScene->GetScreenPosition(itr->beat);
				float cpX = laneCenter + (itr->pos.x / 8.0f) * (laneSize / 2);
				prevPos = { cpX, cpY };
				itr++;
			}
			else {
				break;
			}
		}
	}
}

void EObjArcNote::Select(float x1, float y1, float x2, float y2, bool keep)
{
	ColPrimitive2D::Rect rect({ x1, y1 }, { x2, y2 });
	int32_t laneSize = mScene->sMaxLaneX - mScene->sMinLaneX;
	int32_t laneCenter = (mScene->sMinLaneX + mScene->sMaxLaneX) / 2;
	int32_t arcHeightSize = mScene->sMaxArcHeightX - mScene->sMinArcHeightX;

	//始点
	float startY = mScene->GetScreenPosition(mStartBeat);

	float startX = laneCenter + (mStartPos.x / 8.0f) * (laneSize / 2);
	float startX2 = mScene->sMaxArcHeightX - (((mStartPos.y - 1) / 4.0f) * arcHeightSize);

	ColPrimitive2D::Rect startRect({ startX - 10, startY - 10 }, { startX + 10, startY + 10 });
	ColPrimitive2D::Rect startRect2({ startX2 - 10, startY - 10 }, { startX2 + 10, startY + 10 });
	if (ColPrimitive2D::IsHit(rect, startRect) || ColPrimitive2D::IsHit(rect, startRect2)) {
		if (keep) {
			mSelectStartPos = true;
		}
		else {
			mSelectStartPos = !mSelectStartPos;
		}
		mSelectStartHeight = ColPrimitive2D::IsHit(rect, startRect2);
	}

	//終点
	float endY = mScene->GetScreenPosition(mEndBeat);

	float endX = laneCenter + (mEndPos.x / 8.0f) * (laneSize / 2);
	float endX2 = mScene->sMaxArcHeightX - (((mEndPos.y - 1) / 4.0f) * arcHeightSize);

	ColPrimitive2D::Rect endRect({ endX - 12, endY - 12 }, { endX + 12, endY + 12 });
	ColPrimitive2D::Rect endRect2({ endX2 - 12, endY - 12 }, { endX2 + 12, endY + 12 });
	if (ColPrimitive2D::IsHit(rect, endRect) || ColPrimitive2D::IsHit(rect, endRect2)) {
		if (keep) {
			mSelectEndPos = true;
		}
		else {
			mSelectEndPos = !mSelectEndPos;
		}
		mSelectEndHeight = ColPrimitive2D::IsHit(rect, endRect2);
	}

	//制御点
	for (auto& cp : mControlPoints) {
		float cpY = mScene->GetScreenPosition(cp.beat);

		float cpX = laneCenter + (cp.pos.x / 8.0f) * (laneSize / 2);
		float cpX2 = mScene->sMaxArcHeightX - (((cp.pos.y - 1) / 4.0f) * arcHeightSize);

		ColPrimitive2D::Rect cpRect({ cpX - 12, cpY - 12 }, { cpX + 12, cpY + 12 });
		ColPrimitive2D::Rect cpRect2({ cpX2 - 12, cpY - 12 }, { cpX2 + 12, cpY + 12 });
		if (ColPrimitive2D::IsHit(rect, cpRect) || ColPrimitive2D::IsHit(rect, cpRect2)) {
			if (keep) {
				cp.select = true;
			}
			else {
				cp.select = !cp.select;
			}
			cp.selectHeight = ColPrimitive2D::IsHit(rect, cpRect2);
		}
	}
}

void EObjArcNote::UnSelect()
{
	mSelectStartPos = false;
	mSelectEndPos = false;
	for (auto& cp : mControlPoints) {
		cp.select = false;
	}
}

void EObjArcNote::MoveStart()
{
	int32_t laneSize = mScene->sMaxLaneX - mScene->sMinLaneX;
	int32_t laneCenter = (mScene->sMinLaneX + mScene->sMaxLaneX) / 2;
	int32_t arcHeightSize = mScene->sMaxArcHeightX - mScene->sMinArcHeightX;

	if (mSelectStartPos) {
		if (mSelectStartHeight) {
			mStartPosMoveOrigin.x = mScene->sMaxArcHeightX - (((mStartPos.y - 1) / 4.0f) * arcHeightSize);
		}
		else {
			mStartPosMoveOrigin.x = laneCenter + (mStartPos.x / 8.0f) * (laneSize / 2);
		}
		mStartPosMoveOrigin.y = mScene->GetScreenPosition(mStartBeat);
	}

	if (mSelectEndPos) {
		if (mSelectEndHeight) {
			mEndPosMoveOrigin.x = mScene->sMaxArcHeightX - (((mEndPos.y - 1) / 4.0f) * arcHeightSize);
		}
		else {
			mEndPosMoveOrigin.x = laneCenter + (mEndPos.x / 8.0f) * (laneSize / 2);
		}
		mEndPosMoveOrigin.y = mScene->GetScreenPosition(mEndBeat);
	}

	for (auto& cp : mControlPoints) {
		if (cp.select) {
			if (cp.selectHeight) {
				cp.moveOrigin.x = mScene->sMaxArcHeightX - (((cp.pos.y - 1) / 4.0f) * arcHeightSize);
			}
			else {
				cp.moveOrigin.x = laneCenter + (cp.pos.x / 8.0f) * (laneSize / 2);
			}
			cp.moveOrigin.y = mScene->GetScreenPosition(cp.beat);
		}
	}
}

void EObjArcNote::Move(float dx, float dy)
{
	int32_t laneSize = EditorScene::sMaxLaneX - EditorScene::sMinLaneX;
	int32_t laneCenter = (EditorScene::sMinLaneX + EditorScene::sMaxLaneX) / 2;
	int32_t arcHeightSize = EditorScene::sMaxArcHeightX - EditorScene::sMinArcHeightX;

	if (mSelectStartPos) {
		Vector2 mouse = mStartPosMoveOrigin;
		mouse += {dx, dy};

		if (mSelectStartHeight) {
			if (mouse.x < EditorScene::sMinArcHeightX) {
				mStartPos.y = 5;
			}
			else if (mouse.x > EditorScene::sMaxArcHeightX) {
				mStartPos.y = 1;
			}
			else {
				float nearY = 1;
				float nearDistance = abs(EditorScene::sMaxArcHeightX - mouse.x);
				for (float y = 1; y <= 5; y += 0.25f) {
					float x = EditorScene::sMaxArcHeightX - (((y - 1) / 4.0f) * arcHeightSize);
					float distance = abs(x - mouse.x);
					if (distance <= nearDistance) {
						nearY = y;
						nearDistance = distance;
					}
				}
				mStartPos.y = nearY;
			}
		}
		else {
			if (mouse.x < EditorScene::sMinLaneX) {
				mStartPos.x = -8;
			}
			else if (mouse.x > EditorScene::sMaxLaneX) {
				mStartPos.x = 8;
			}
			else {
				float nearX = 1;
				float nearDistance = abs(EditorScene::sMinLaneX - mouse.x);
				for (float x = -8; x <= 8; x += 0.25f) {
					float sx = laneCenter + (x / 8.0f) * (laneSize / 2);
					float distance = abs(sx - mouse.x);
					if (distance <= nearDistance) {
						nearX = x;
						nearDistance = distance;
					}
				}
				mStartPos.x = nearX;
			}
		}
		mStartBeat = mScene->GetVirtualCursorBeat(mouse.y);
	}

	if (mSelectEndPos) {
		Vector2 mouse = mEndPosMoveOrigin;
		mouse += {dx, dy};

		if (mSelectEndHeight) {
			if (mouse.x < EditorScene::sMinArcHeightX) {
				mEndPos.y = 5;
			}
			else if (mouse.x > EditorScene::sMaxArcHeightX) {
				mEndPos.y = 1;
			}
			else {
				float nearY = 1;
				float nearDistance = abs(EditorScene::sMaxArcHeightX - mouse.x);
				for (float y = 1; y <= 5; y += 0.25f) {
					float x = EditorScene::sMaxArcHeightX - (((y - 1) / 4.0f) * arcHeightSize);
					float distance = abs(x - mouse.x);
					if (distance <= nearDistance) {
						nearY = y;
						nearDistance = distance;
					}
				}
				mEndPos.y = nearY;
			}
		}
		else {
			if (mouse.x < EditorScene::sMinLaneX) {
				mEndPos.x = -8;
			}
			else if (mouse.x > EditorScene::sMaxLaneX) {
				mEndPos.x = 8;
			}
			else {
				float nearX = 1;
				float nearDistance = abs(EditorScene::sMinLaneX - mouse.x);
				for (float x = -8; x <= 8; x += 0.25f) {
					float sx = laneCenter + (x / 8.0f) * (laneSize / 2);
					float distance = abs(sx - mouse.x);
					if (distance <= nearDistance) {
						nearX = x;
						nearDistance = distance;
					}
				}
				mEndPos.x = nearX;
			}
		}
		mEndBeat = mScene->GetVirtualCursorBeat(mouse.y);
	}

	for (auto& cp : mControlPoints) {
		if (cp.select) {
			Vector2 mouse = cp.moveOrigin;
			mouse += {dx, dy};

			if (cp.selectHeight) {
				if (mouse.x < EditorScene::sMinArcHeightX) {
					cp.pos.y = 5;
				}
				else if (mouse.x > EditorScene::sMaxArcHeightX) {
					cp.pos.y = 1;
				}
				else {
					float nearY = 1;
					float nearDistance = abs(EditorScene::sMaxArcHeightX - mouse.x);
					for (float y = 1; y <= 5; y += 0.25f) {
						float x = EditorScene::sMaxArcHeightX - (((y - 1) / 4.0f) * arcHeightSize);
						float distance = abs(x - mouse.x);
						if (distance <= nearDistance) {
							nearY = y;
							nearDistance = distance;
						}
					}
					cp.pos.y = nearY;
				}
			}
			else {
				if (mouse.x < EditorScene::sMinLaneX) {
					cp.pos.x = -8;
				}
				else if (mouse.x > EditorScene::sMaxLaneX) {
					cp.pos.x = 8;
				}
				else {
					float nearX = 1;
					float nearDistance = abs(EditorScene::sMinLaneX - mouse.x);
					for (float x = -8; x <= 8; x += 0.25f) {
						float sx = laneCenter + (x / 8.0f) * (laneSize / 2);
						float distance = abs(sx - mouse.x);
						if (distance <= nearDistance) {
							nearX = x;
							nearDistance = distance;
						}
					}
					cp.pos.x = nearX;
				}
			}
			cp.beat = mScene->GetVirtualCursorBeat(mouse.y);
		}
	}

	std::stable_sort(mControlPoints.begin(), mControlPoints.end(), [](EObjArcNote::ControlPoint const& lhs, EObjArcNote::ControlPoint const& rhs) {
		return lhs.beat < rhs.beat;
	});
}

void EObjArcNote::Draw()
{
	int32_t laneSize = mScene->sMaxLaneX - mScene->sMinLaneX;
	int32_t laneCenter = (mScene->sMinLaneX + mScene->sMaxLaneX) / 2;
	int32_t arcHeightSize = mScene->sMaxArcHeightX - mScene->sMinArcHeightX;

	Vector3 prevPos = mStartPos;
	prevPos.z = mScene->GetScreenPosition(mStartBeat);

	float startX = laneCenter + (prevPos.x / 8.0f) * (laneSize / 2);
	float startX2 = mScene->sMaxArcHeightX - (((prevPos.y - 1) / 4.0f) * arcHeightSize);

	SimpleDrawer::DrawBox(startX - 10, prevPos.z - 10, startX + 10, prevPos.z + 10, 10, 0xaa33aa, true);
	SimpleDrawer::DrawBox(startX2 - 10, prevPos.z - 10, startX2 + 10, prevPos.z + 10, 10, 0xaa33aa, true);
	if (mSelectStartPos) {
		SimpleDrawer::DrawBox(startX - 10, prevPos.z - 10, startX + 10, prevPos.z + 10, 10, 0xff0000, false, 5);
		SimpleDrawer::DrawBox(startX2 - 10, prevPos.z - 10, startX2 + 10, prevPos.z + 10, 10, 0xff0000, false, 5);
	}

	Vector3 startPos;
	Vector3 endPos;
	bool select = false;
	bool selectHeight = false;

	auto itr = mControlPoints.begin();

	while (true) {
		startPos = prevPos;
		if (itr != mControlPoints.end()) {
			endPos = itr->pos;
			endPos.z = mScene->GetScreenPosition(itr->beat);
			select = itr->select;
			selectHeight = itr->selectHeight;
		}
		else {
			endPos = mEndPos;
			endPos.z = mScene->GetScreenPosition(mEndBeat);
			select = mSelectEndPos;
			selectHeight = mSelectEndHeight;
		}

		if ((startPos.z < 0 && endPos.z < 0) || (startPos.z < 0 && endPos.z > 720)) {
			if (itr != mControlPoints.end()) {
				prevPos = itr->pos;
				prevPos.z = mScene->GetScreenPosition(itr->beat);
				itr++;
				continue;
			}
			else {
				break;
			}
		}

		int32_t x1 = static_cast<int32_t>(laneCenter + (startPos.x / 8.0f) * (laneSize / 2));
		int32_t x2 = static_cast<int32_t>(laneCenter + (endPos.x / 8.0f) * (laneSize / 2));
		int32_t x3 = static_cast<int32_t>(mScene->sMaxArcHeightX - ((startPos.y - 1) / 4.0f) * arcHeightSize);
		int32_t x4 = static_cast<int32_t>(mScene->sMaxArcHeightX - ((endPos.y - 1) / 4.0f) * arcHeightSize);

		SimpleDrawer::DrawCircle(x2, static_cast<int32_t>(endPos.z), 12, 10, 0xaa33aa, true);
		SimpleDrawer::DrawLine(x1, static_cast<int32_t>(startPos.z), x2, static_cast<int32_t>(endPos.z), 10, 0xaa33aa, 5);
		SimpleDrawer::DrawCircle(x4, static_cast<int32_t>(endPos.z), 12, 10, 0xaa33aa, true);
		SimpleDrawer::DrawLine(x3, static_cast<int32_t>(startPos.z), x4, static_cast<int32_t>(endPos.z), 10, 0xaa33aa, 5);
		if (select) {
			SimpleDrawer::DrawCircle(x2, static_cast<int32_t>(endPos.z), 12, 10, 0xff0000, false, 5);
			SimpleDrawer::DrawCircle(x4, static_cast<int32_t>(endPos.z), 12, 10, 0xff0000, false, 5);
		}

		if (itr != mControlPoints.end()) {
			prevPos = itr->pos;
			prevPos.z = mScene->GetScreenPosition(itr->beat);
			itr++;
		}
		else {
			break;
		}
	}
}

void EObjArcNote::DrawToMiniMap()
{
	float multipiler = 720 / mScene->GetAllScreenLength();
	int32_t laneSize = 1280 - 1180;
	int32_t laneCenter = (1180 + 1280) / 2;

	Vector3 prevPos = mStartPos;
	prevPos.z = 720 - mScene->GetOriginScreenPosition(mStartBeat) * multipiler;

	//float startX = laneCenter + (prevPos.x / 8.0f) * (laneSize / 2);

	//SimpleDrawer::DrawBox(startX - 10, prevPos.z - 10, startX + 10, prevPos.z + 10, 10, 0xaa33aa, true);

	Vector3 startPos;
	Vector3 endPos;
	bool select = false;
	bool selectHeight = false;

	auto itr = mControlPoints.begin();

	while (true) {
		startPos = prevPos;
		if (itr != mControlPoints.end()) {
			endPos = itr->pos;
			endPos.z = 720 - mScene->GetOriginScreenPosition(itr->beat) * multipiler;
			select = itr->select;
			selectHeight = itr->selectHeight;
		}
		else {
			endPos = mEndPos;
			endPos.z = 720 - mScene->GetOriginScreenPosition(mEndBeat) * multipiler;
			select = mSelectEndPos;
			selectHeight = mSelectEndHeight;
		}

		if ((startPos.z < 0 && endPos.z < 0) || (startPos.z < 0 && endPos.z > 720)) {
			if (itr != mControlPoints.end()) {
				prevPos = itr->pos;
				prevPos.z = 720 - mScene->GetOriginScreenPosition(itr->beat) * multipiler;
				itr++;
				continue;
			}
			else {
				break;
			}
		}

		int32_t x1 = static_cast<int32_t>(laneCenter + (startPos.x / 8.0f) * (laneSize / 2));
		int32_t x2 = static_cast<int32_t>(laneCenter + (endPos.x / 8.0f) * (laneSize / 2));

		//SimpleDrawer::DrawCircle(x2, static_cast<int32_t>(endPos.z), 12, 10, 0xaa33aa, true);
		float thickness = 5.0f * multipiler;
		if (thickness < 1.0f) thickness = 1.0f;
		SimpleDrawer::DrawLine(x1, static_cast<int32_t>(startPos.z), x2, static_cast<int32_t>(endPos.z), 10, 0xaa33aa, thickness);

		if (itr != mControlPoints.end()) {
			prevPos = itr->pos;
			prevPos.z = 720 - mScene->GetOriginScreenPosition(itr->beat) * multipiler;
			itr++;
		}
		else {
			break;
		}
	}
}

void EObjArcNote::TempDraw(EditorScene* scene, float x, Beat beat)
{
	int32_t laneSize = EditorScene::sMaxLaneX - EditorScene::sMinLaneX;
	int32_t laneCenter = (EditorScene::sMinLaneX + EditorScene::sMaxLaneX) / 2;

	float sx = laneCenter + (x / 8.0f) * (laneSize / 2);
	float sy = scene->GetScreenPosition(beat);

	if (sy < 0 || sy > 720) {
		return;
	}

	SimpleDrawer::DrawBox(sx - 10, sy - 10, sx + 10, sy + 10, 10, Color(0.66f, 0.2f, 0.66f, 0.3f), true);
}
