#pragma once
#include "Singleton.h"
#include "CollectPart.h"
#include "CollectZone.h"
#include <memory>

class CollectPartManager : public Singleton
{
public:
	static void LoadResouces();
	void Init();
	void Update();
	void Draw();

	void Craete(const Vector3& spawnPos);

	void ImGui();

	static CollectPartManager* GetInstance();

	std::vector<std::unique_ptr<CollectPart>> parts;

	CollectZone zone;

	int32_t maxCarryingNum = 1;
};

