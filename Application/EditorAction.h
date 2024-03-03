/*
* @file EditorAction.h
* @brief 譜面エディタ上で行った操作一つ一つを記憶するクラス
* Undo, Redo機能用
*/

#pragma once
#include <string>
#include <list>
#include <memory>
#include <IEditorObject.h>

struct EditorAction
{
	std::string name;
	bool saved = false;

	EditorAction() {}
	virtual ~EditorAction() {};

	virtual void Undo(std::list<std::unique_ptr<IEditorObject>>* eoList) = 0;
	virtual void Redo(std::list<std::unique_ptr<IEditorObject>>* eoList) = 0;
};

struct EAAddEO : public EditorAction
{
	std::string uuid;
	std::unique_ptr<IEditorObject> data;

	EAAddEO(std::string uuid, const std::unique_ptr<IEditorObject>& obj)
		: uuid(uuid), data(obj->Clone()) {}

	virtual void Undo(std::list<std::unique_ptr<IEditorObject>>* eoList) override {
		for (auto itr = eoList->begin(); itr != eoList->end(); itr++) {
			std::unique_ptr<IEditorObject>& ptr = (*itr);
			if (ptr->mUuid == uuid) {
				itr = eoList->erase(itr);
				break;
			}
		}
	}

	virtual void Redo(std::list<std::unique_ptr<IEditorObject>>* eoList) override {
		eoList->push_back(data->Clone());
	}
};

struct EARemoveEO : public EditorAction
{
	std::string uuid;
	std::unique_ptr<IEditorObject> data;

	EARemoveEO(std::string uuid, const std::unique_ptr<IEditorObject>& obj)
		: uuid(uuid), data(obj->Clone()) {}

	virtual void Undo(std::list<std::unique_ptr<IEditorObject>>* eoList) override {
		eoList->push_back(data->Clone());
	}

	virtual void Redo(std::list<std::unique_ptr<IEditorObject>>* eoList) override {
		for (auto itr = eoList->begin(); itr != eoList->end(); itr++) {
			std::unique_ptr<IEditorObject>& ptr = (*itr);
			if (ptr->mUuid == uuid) {
				itr = eoList->erase(itr);
				break;
			}
		}
	}
};

template<typename T>
struct EAModifyEOField : public EditorAction
{
	std::string targetUUID;
	size_t addressPos;
	T data;

	EAModifyEOField(std::string uuid, size_t addressPos, T data)
		: targetUUID(uuid), addressPos(addressPos), data(data) {}

	virtual void Undo(std::list<std::unique_ptr<IEditorObject>>* eoList) override {
		for (auto itr = eoList->begin(); itr != eoList->end(); itr++) {
			std::unique_ptr<IEditorObject>& ptr = (*itr);
			if (ptr->mUuid == targetUUID) {
				uint8_t* targetAddress = reinterpret_cast<uint8_t*>(ptr.get()) + addressPos;
				T* target = reinterpret_cast<T*>(targetAddress);

				T temp = *target;
				*target = data;
				data = temp;
				break;
			}
		}
	}

	virtual void Redo(std::list<std::unique_ptr<IEditorObject>>* eoList) override {
		Undo(eoList);
	}
};