#pragma once

class Boss;

enum class BossSituation
{
	All,
	OnlyLeft,
	OnlyRight,
	NoArms,
	Appearance,	//出現
};

class BossAI
{
private:
	bool isStart;

	BossSituation situation = BossSituation::All; //想定されるボスの遷移状態

public:
	void Init();
	void Update(Boss* boss);

	BossSituation GetSituation();

	void SetSituation(const BossSituation& situation);
};