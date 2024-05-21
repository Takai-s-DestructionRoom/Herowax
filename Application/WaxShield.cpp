#include "WaxShield.h"
#include "Camera.h"
#include "WaxManager.h"
#include "TimeManager.h"

void WaxShield::Init()
{
	obj = PaintableModelObj(Model::Load("./Resources/Model/Shield/shield.obj", "shield"));
}

void WaxShield::Update()
{
	UpdateCollider();

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	obj.mPaintDataBuff->dissolveVal = IsSolid() ? 1.0f : 0.3f / (requireWaxSolidCount - 1) * waxSolidCount;
	//obj.mPaintDataBuff->dissolveVal = 1.0f;
	obj.mPaintDataBuff->color = WaxManager::GetInstance()->slimeWax.waxColor;
	obj.mPaintDataBuff->slide += TimeManager::deltaTime;

	obj.mTransform.UpdateMatrix();
	BrightTransferBuffer(Camera::sNowCamera->mViewProjection);
}

void WaxShield::Draw()
{
	BrightDraw();
}

void WaxShield::Hit(int32_t damage)
{
	waxSolidCount += damage;
}

bool WaxShield::IsSolid()
{
	return waxSolidCount >= requireWaxSolidCount;
}
