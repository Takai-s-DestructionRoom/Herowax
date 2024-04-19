#pragma once
#include "Singleton.h"
#include "CollectPart.h"
#include "CollectZone.h"
#include <memory>

class Player;

class CollectPartManager : public Singleton
{
public:
	static CollectPartManager* GetInstance();
	
	static void LoadResouces();
	
	void SetPlayer(Player* player_) { player = player_; };

	void Init();
	void Update();
	void Draw();

	void Craete(const Vector3& spawnPos);

	bool GetAllowCreate();

	void CratePostDelete();

	int32_t GetMaxCarryingNum() {return maxCarryingNum;};

	void ImGui();

	int32_t GetCarryingNum();

	void CreateReward();

public:
	std::vector<std::unique_ptr<CollectPart>> parts;

	CollectZone zone;

private:
	int32_t maxCarryingNum = 1;

	int32_t requireCreateNum = 10;

	Player* player = nullptr;
};

