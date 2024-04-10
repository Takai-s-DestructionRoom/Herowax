#pragma once

class Boss;

class BossAI
{
private:
	bool isStart;

public:
	void Init();
	void Update(Boss* boss);
};