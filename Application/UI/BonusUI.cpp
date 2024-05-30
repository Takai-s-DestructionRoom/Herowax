#include "BonusUI.h"

void BonusUI::Init()
{
	circleGauge.Init("bonusGauge");

	bonusText = TextureManager::Load("./Resources/UI/bonusText.png", "bonusText");
	bonusGetText = TextureManager::Load("./Resources/UI/bonusGetText.png", "bonusGetText");
}

void BonusUI::Update()
{
	circleGauge.baseRadian = Util::Clamp(circleGauge.baseRadian, 0.0f, 360.f);
	
	circleGauge.ImGui();
	circleGauge.Update();

	bonusText.mTransform.position = circleGauge.baseTrans.position;
	bonusText.mTransform.position.y += 10.f;
	
	bonusGetText.mTransform.position = circleGauge.baseTrans.position;
	bonusGetText.mTransform.position.y += 10.f;

	bonusText.mTransform.UpdateMatrix();
	bonusText.TransferBuffer();

	bonusGetText.mTransform.UpdateMatrix();
	bonusGetText.TransferBuffer();
}

void BonusUI::Draw()
{
	circleGauge.Draw();

	if (circleGauge.baseRadian <= 0.0f) {
		bonusGetText.Draw();
	}
	else if(circleGauge.baseRadian < 360.f){
		bonusText.Draw();
	}
}
