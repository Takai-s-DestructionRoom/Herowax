#include "WaxState.h"
#include "Wax.h"
#include "Easing.h"

void WaxNormal::Update(Wax* wax)
{
	wax;
}

void WaxIgnite::Update(Wax* wax)
{
	if (!wax->igniteTimer.GetStarted())wax->igniteTimer.Start();
	wax->igniteTimer.Update();

	//色が変わる
	wax->obj.mTuneMaterial.mColor.r = Easing::InQuad(wax->waxOriginColor.r, wax->waxEndColor.r, wax->igniteTimer.GetTimeRate());
	wax->obj.mTuneMaterial.mColor.g = Easing::InQuad(wax->waxOriginColor.g, wax->waxEndColor.g, wax->igniteTimer.GetTimeRate());
	wax->obj.mTuneMaterial.mColor.b = Easing::InQuad(wax->waxOriginColor.b, wax->waxEndColor.b, wax->igniteTimer.GetTimeRate());

	//次へ
	if (wax->igniteTimer.GetEnd()) {
		wax->ChangeState(new WaxBurining());
	}
}

void WaxBurining::Update(Wax* wax)
{
	if (!wax->burningTimer.GetStarted())wax->burningTimer.Start();
	wax->burningTimer.Update();
	//燃える、この状態で当たったら周囲をigniteにする

	//次へ
	if (wax->burningTimer.GetEnd()) {
		wax->ChangeState(new WaxExtinguish());
	}
}

void WaxExtinguish::Update(Wax* wax)
{
	if (!wax->extinguishTimer.GetStarted()) {
		wax->extinguishTimer.Start();
		saveScale = wax->GetScale();
	}
	wax->extinguishTimer.Update();
	//縮小する
	wax->obj.mTransform.scale.x = Easing::OutQuad(saveScale.x, 0, wax->extinguishTimer.GetTimeRate());
	wax->obj.mTransform.scale.y = Easing::OutQuad(saveScale.y, 0, wax->extinguishTimer.GetTimeRate());
	wax->obj.mTransform.scale.z = Easing::OutQuad(saveScale.z, 0, wax->extinguishTimer.GetTimeRate());

	//終わったら死亡
	if (wax->extinguishTimer.GetEnd()) {
		wax->SetIsAlive(false);
	}
}