#pragma once
#include "Vector3.h"
#include "Vector2.h"
#include <string>
#include <vector>

/*! LevelData
	blenderアドオンから出力したjsonファイルからステージ情報
*/
struct LevelData
{
	struct ColliderData {
		Vector3 center{};
		Vector3 size{};
		bool have = false;
	};
	struct ObjectData {
		std::string setObjectName;
		std::string spawnerOrder;
		
		Vector3 translation{};
		Vector3 rotation{};
		Vector3 scaling{};

		ColliderData collider{};
	};

	std::vector<ObjectData> mObjects;

	bool isCreate = false;

	std::string mHandle = "";
	std::string mPath = "";
};

