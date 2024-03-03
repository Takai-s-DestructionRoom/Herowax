/*
* @file EObjArcNote.h
* @brief 譜面エディタ上におけるレーザーノートのクラス
*/

#pragma once
#include "IEditorObject.h"
#include <BeatUtil.h>
#include <Vector2.h>

class EObjArcNote : public IEditorObject
{
public:
	EObjArcNote(EditorScene* scene);
	std::string GetTypeName() override { return "EObjArcNote"; };
	std::unique_ptr<IEditorObject> Clone() const override;
	std::unique_ptr<EditorAction> GetSavePoint() override;
	Vector2 GetPos() override;
	Beat GetBeat() override;

	struct ControlPoint {
		Beat beat;
		Vector2 pos;
		bool select = false;
		bool selectHeight = false;
		Vector2 moveOrigin;
	};

	Beat mStartBeat = { 0, 0, 1 };
	Beat mEndBeat = { 0, 0, 1 };
	Vector2 mStartPos = { 0, 0 };
	Vector2 mEndPos = { 0, 0 };

	//これはBeatが早い順にソートされているものとします！！！
	std::list<ControlPoint> mControlPoints;

	bool mSelectStartPos = false;
	bool mSelectStartHeight = false;
	bool mSelectEndPos = false;
	bool mSelectEndHeight = false;

	Vector2 mStartPosMoveOrigin;
	Vector2 mEndPosMoveOrigin;

	bool Collide(float x, float y) override;
	bool Collide(float x1, float y1, float x2, float y2) override;

	bool IsSelected() override;
	int32_t GetSelectCountInThisInstance();
	void Select(float x, float y, bool keep) override;
	void Select(float x1, float y1, float x2, float y2, bool keep) override;
	void UnSelect() override;

	void MoveStart() override;
	void Move(float x, float y) override;
	
	void Draw() override;
	void DrawToMiniMap() override;

	static void TempDraw(EditorScene* scene, float x, Beat beat);
};

