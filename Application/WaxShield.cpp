#include "WaxShield.h"
#include "Camera.h"
#include "WaxManager.h"
#include "TimeManager.h"
#include "ParticleManager.h"

void WaxShield::Init()
{
	obj = PaintableModelObj(Model::Load("./Resources/Model/Shield/shield.obj", "shield"));
	obj.mPaintDissolveMapTex = TextureManager::Load("./Resources/DissolveMap.png", "DissolveMapTex");

}

void WaxShield::Update()
{
	//もし固まったら
	if (IsSolid() && isAlive) {
		//死亡
		isAlive = false;
		//破壊パーティクル発生
		ParticleManager::GetInstance()->AddSimple(obj.mTransform.position, "barrier_crash_mini");
	}

	if (!isAlive)return;

	UpdateCollider();

	brightTimer.Update();
	if (brightTimer.GetStarted()) {
		brightColor.r = Easing::OutQuad(1.0f, 0.0f, brightTimer.GetTimeRate());
		brightColor.g = Easing::OutQuad(1.0f, 0.0f, brightTimer.GetTimeRate());
		brightColor.b = Easing::OutQuad(1.0f, 0.0f, brightTimer.GetTimeRate());
	}
	
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
	if (!isAlive)return;

	BrightDraw();
	/*isDrawCollider = true;
	DrawCollider();*/
}

void WaxShield::Hit(int32_t damage)
{
	waxSolidCount += damage;
	brightTimer.Start();
	brightColor = { 1,1,1,0};

}

bool WaxShield::IsSolid()
{
	return waxSolidCount >= requireWaxSolidCount;
}
