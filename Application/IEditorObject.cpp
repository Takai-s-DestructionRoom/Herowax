#include "IEditorObject.h"

IEditorObject::IEditorObject(EditorScene* scene)
	: mScene(scene)
{
	mUuid = std::to_string((int64_t)rand() * (int64_t)rand());
}
