#include "WaxWall.h"

void WaxWall::Init()
{
	obj = PaintableModelObj(Model::Load("./Resources/Model/Shield/shield.obj", "shield"));
	obj.mTransform.scale = { 2,2,2 };

	colliderSize = 5;

	isAlive = false;
}

void WaxWall::Update()
{
	//パリィ中の場合はパリィが終わった後に終了を呼ぶ
	if (endFlag && parryTimer.GetEnd()) {
		End();
		endFlag = false;
	}

	parryTimer.Update();
	
	if (parryTimer.GetNowEnd()) {
		leakOutTimer.Start();
	}

	leakOutTimer.Update();

	if (!isAlive)return;

	UpdateCollider();

	obj.mTransform.UpdateMatrix();
	BrightTransferBuffer(Camera::sNowCamera->mViewProjection);
}

void WaxWall::Draw()
{
	if (!isAlive)return;

	BrightDraw();

	isDrawCollider = true;
	DrawCollider();
}

bool WaxWall::GetParry()
{
	return parryTimer.GetRun();
}

bool WaxWall::GetLeakOutMode()
{
	return parryTimer.GetEnd();
}

bool WaxWall::ContinueCheck(int32_t remainWax)
{
	//漏れるタイマーが終了して
	if (leakOutTimer.GetEnd()) {
		//ロウの消費ができるなら継続
		if(remainWax >= CONTINUE_CHECK_WAXNUM){
			leakOutTimer.Start();
			return true;
		}
		//出来ないなら終了
		else {
			End();
		}
	}
	return false;
}

bool WaxWall::StartCheck(int32_t remainWax)
{
	return remainWax >= START_CHECK_WAXNUM;
}

void WaxWall::Start()
{
	parryTimer.Start();
	isAlive = true;
	endFlag = false;
}

void WaxWall::End()
{
	if (parryTimer.GetEnd()) {
		isAlive = false;
		leakOutTimer.Reset();
		parryTimer.Reset();
	}
	else {
		endFlag = true;
	}
}
