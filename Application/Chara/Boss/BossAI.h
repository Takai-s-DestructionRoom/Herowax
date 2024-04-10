#pragma once

class Boss;

class BossAI
{
private:
	float actTimer = 0;

public:
	void Init();
	void Update(Boss* boss);
};