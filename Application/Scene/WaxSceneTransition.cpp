#include "WaxSceneTransition.h"
#include "RWindow.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include "MetaBall2D.h"
#include "Parameter.h"

WaxSceneTransition::WaxSceneTransition()
{
	MetaBall2DManager::GetInstance()->Init();

	openTimer.Reset();
	closeTimer.Start();

	std::map<std::string, std::string> extract =  Parameter::Extract("WaxSceneTransition");
	
	for (int32_t i = 0; i < (int32_t)Parameter::GetParam(extract, "num", 0.f); i++)
	{
		brushs.emplace_back();
		brushs.back().Init();
		std::string string = "pos_";
		string += std::to_string(i);
		brushs.back().sprite.mTransform.position = Parameter::GetVector3Data(extract, string, {0.0f,0.0f,0.0f});
		string = "scale_";
		string += std::to_string(i);
		brushs.back().sprite.mTransform.scale = Parameter::GetVector3Data(extract, string, {0.0f,0.0f,0.0f});
		brushs.back().sizeMax = Parameter::GetVector3Data(extract, string, {0.0f,0.0f,0.0f});
	}

	nowNum = 0;
}

void WaxSceneTransition::Update()
{
	MetaBall2DManager::GetInstance()->Update();

	for (auto& brush : brushs)
	{
		brush.Update();
	}

	//超えたら
	//ループを止める
	brushTimer.Roop();
	if (brushTimer.GetRoopEnd() && (nowNum < num)) {
		brushs[nowNum].Start({0.f,0.f },
			{ brushs[nowNum].sizeMax.x,brushs[nowNum].sizeMax.y});
		nowNum++;
	}

	openTimer.Update();
	closeTimer.Update();

	//閉じてくよって時
	if (mIsClosed) {
		if (closeTimer.GetEnd()) {		//タイマー終わったら
			inProgress = false;			//進行終了
			openTimer.Start();
		}
		else {
			inProgress = true;		//進行中
		}
	}
	//開いてくよって時
	else {
		if (openTimer.GetEnd()) {
			inProgress = false;
		}
		else {
			inProgress = true;
		}
	}
}

void WaxSceneTransition::Draw()
{
	MetaBall2DManager::GetInstance()->Draw();

	for (auto& brush : brushs)
	{
		brush.Draw();
	}
}

void WaxSceneTransition::Open()
{
	mIsClosed = false;
	openTimer.Start();
	closeTimer.SetEnd(true);

	/*for (int32_t i = 0;i < MetaBall2DManager::GetInstance()->METABALL_NUM; i++)
	{
		MetaBall2DManager::GetInstance()->metaballs[i]->isUse = false;
	}*/

	brushs.clear();
}

bool WaxSceneTransition::IsOpened()
{
	return openTimer.GetEnd();
}

void WaxSceneTransition::Close()
{
	mIsClosed = true;
	openTimer.Reset();
	closeTimer.Start();
}

bool WaxSceneTransition::IsClosed()
{
	return closeTimer.GetEnd();
}

bool WaxSceneTransition::InProgress()
{
	return inProgress;
}
