#pragma once
#pragma once
#include "BossState.h"

class BossAppearance : public BossState
{
public:
	BossAppearance();
	void Update(Boss* boss)override;

private:

};

