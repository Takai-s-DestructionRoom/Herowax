#include "WaxGroup.h"
#include "WaxManager.h"

void WaxGroup::Init()
{
}

void WaxGroup::Update()
{
	//死んだ蝋がいたら全削除
	for (uint32_t i = 0; i < waxNums.size(); i++)
	{
		if (WaxManager::GetInstance()->waxs[waxNums[i]]->GetIsAlive() == false)
		{
			waxNums.clear();
		}
	}
}

void WaxGroup::Draw()
{
}
