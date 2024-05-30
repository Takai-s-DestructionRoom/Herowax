#include "WaxSceneTransition.h"
#include "RWindow.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include "MetaBall2D.h"

WaxSceneTransition::WaxSceneTransition()
{
	MetaBall2DManager::GetInstance()->Init();

	openTimer.Reset();
	closeTimer.Start();
}

void WaxSceneTransition::Update()
{
	MetaBall2DManager::GetInstance()->Update();
	

	MetaBall2DManager::GetInstance()->CraeteMetaBall();

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
}

void WaxSceneTransition::Open()
{
	mIsClosed = false;
	openTimer.Start();
	closeTimer.SetEnd(true);
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
