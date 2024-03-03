/*
* @file　IEditorObject.h
* @brief 譜面エディタ上におけるオブジェクトの基底クラス
* 選択とかD&Dとか諸々の挙動はこれを基に各クラスで実装する
*/

#pragma once
#include <string>
#include <memory>

#include <BeatUtil.h>
#include <Vector2.h>

class EditorScene;
struct EditorAction;

class IEditorObject
{
public:
	EditorScene* mScene;
	std::string mUuid;
	bool mShowDetailWindow = false;

	IEditorObject(EditorScene* scene);
	virtual ~IEditorObject() {};

	virtual std::string GetTypeName() = 0;
	virtual std::unique_ptr<IEditorObject> Clone() const = 0;
	virtual std::unique_ptr<EditorAction> GetSavePoint() = 0;

	virtual Beat GetBeat() { return { 0, 0, 1 }; };
	virtual Vector2 GetPos() { return { 0, 0 }; };

	//当たり判定
	virtual bool Collide(float x, float y) { x; y; return false; };
	virtual bool Collide(float x1, float y1, float x2, float y2) { x1; y1; x2; y2; return false; };

	//選択されているか
	virtual bool IsSelected() { return false; };

	//この一つのインスタンス内で選択されているオブジェクトが何個分か
	//例えばレーザーノートの場合は本体以外に制御点の選択が可能なので、
	//その分をカウントする
	virtual int32_t GetSelectCountInThisInstance() { return 0; }

	//選択
	virtual void Select(float x, float y, bool keep) { x; y; keep; };
	virtual void Select(float x1, float y1, float x2, float y2, bool keep) { x1; y1; x2; y2; keep; };
	virtual void UnSelect() {};

	//削除時処理
	virtual void OnErase() {};

	virtual void Update() {};
	virtual void SelectUpdate() {};

	virtual void MoveStart() {};
	virtual void Move(float x, float y) { x; y; };

	virtual void Draw() {};
	virtual void DrawToMiniMap() {};
	virtual void SelectDraw() {};
};

