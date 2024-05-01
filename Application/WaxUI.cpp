#include "WaxUI.h"
#include "InstantDrawer.h"

void WaxUI::Init()
{
	TextureManager::Load("./Resources/up!!.png", "up!!");
}

void WaxUI::Update(Vector3 target)
{
	posTimer.Update();
	sizeTimer.Update();

	upPaintPosStart = target;
	upPaintPosEnd = target;
	upPaintPosEnd.y = upPaintPosStart.y + 10;

	upPaintPos = OutQuadVec3(upPaintPosStart, upPaintPosEnd, posTimer.GetTimeRate());

	if (posTimer.GetTimeRate() > 0.7f) {
		if (!sizeTimer.GetStarted())sizeTimer.Start();
	}

	if (sizeTimer.GetStarted()) {
		upPaintSize.x = Easing::OutQuad(upPaintSizeStart.x, 0, sizeTimer.GetTimeRate());
		upPaintSize.y = Easing::OutQuad(upPaintSizeStart.y, 0, sizeTimer.GetTimeRate());
		angle = Easing::OutQuad(0,Util::AngleToRadian(360.f), sizeTimer.GetTimeRate());
	}
}

void WaxUI::Draw()
{
	InstantDrawer::DrawGraph3D(upPaintPos, upPaintSize.x, upPaintSize.y, "up!!", {1,1,1,1}, angle);
}

void WaxUI::Start()
{
	//ここで動かす
	posTimer.Start();
	sizeTimer.Reset();

	upPaintSize = upPaintSizeStart;
	angle = 0;
}
