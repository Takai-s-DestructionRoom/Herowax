#include "PlayerHpUI.h"
#include "RImGui.h"
#include "Player.h"
#include "InstantDrawer.h"
#include "Parameter.h"
#include "GameCamera.h"

void PlayerHpUI::Init()
{
	circleGauge.Init();
	circleGauge.SetTexture(TextureManager::Load("./Resources/UI/hpGauge.png", "hpGauge"));
	circleGauge.SetColor(Color::kGreen);

	circleGaugeDam.Init();
	circleGaugeDam.SetTexture(TextureManager::Load("./Resources/UI/hpGauge.png", "hpGauge"));
	circleGaugeDam.SetColor(Color::kRed);

	circleGaugeBack.Init();
	circleGaugeBack.SetTexture(TextureManager::Load("./Resources/UI/hpGauge.png", "hpGauge"));
	circleGaugeBack.SetColor({0.1f,0.1f, 0.1f, 1.0f});
	
	circleGaugeFrame.Init();
	circleGaugeFrame.SetTexture(TextureManager::Load("./Resources/UI/hpGaugeFrame.png", "hpGaugeFrame"));

	std::map<std::string, std::string> extract = Parameter::Extract("playerHPUI");
	rate = Parameter::GetParam(extract,"レート", rate);
	circleGauge.angle = Parameter::GetParam(extract,"角度", circleGauge.angle);
	angleCompensNum = Parameter::GetParam(extract,"angleCompensNum", angleCompensNum);
	circleGauge.baseTrans.scale = { 1,1,1 };
	circleGaugeBack.baseTrans.scale = { 1,1,1 };
}

void PlayerHpUI::Update()
{
	baseMax = 360.f * rate;
	baseMin = 360.f - baseMax;

	if (player) {
		baseRadian = (1.0f - (player->hp / player->maxHP)) * 360.f;

		oldRadian = baseRadianDam;
		baseRadianDam += (baseRadian - oldRadian) / 15.0f;

		Vector3 gaugePos = player->obj.mTransform.position;
	
		circleGauge.baseTrans.position = Util::GetScreenPos(gaugePos);
		
		float camX = GameCamera::GetInstance()->GetCameraAngle().x;
		camX = Util::RadianToAngle(camX);
		float angleRate = 1.0f - (camX - 15.f) / 90.f;
		playerRangeY = angleRate * -angleCompensNum;
		
		circleGauge.baseTrans.position.y += playerRangeY;

		//15~89までの間で、
		//0~-120まで遷移させる
	}

	circleGauge.baseRadian = baseMin + baseRadian * rate;
	circleGauge.Update();

	circleGaugeDam.baseRadian = baseMin + baseRadianDam * rate;
	circleGaugeDam.baseTrans.position = circleGauge.baseTrans.position;
	circleGaugeDam.angle = circleGauge.angle;
	circleGaugeDam.Update();

	circleGaugeBack.baseRadian = baseMin;
	circleGaugeBack.baseTrans.position = circleGauge.baseTrans.position;
	circleGaugeBack.angle = circleGauge.angle;
	circleGaugeBack.Update();
	
	circleGaugeFrame.baseRadian = baseMin;
	circleGaugeFrame.baseTrans.position = circleGauge.baseTrans.position;
	circleGaugeFrame.angle = circleGauge.angle;
	circleGaugeFrame.Update();

	if (RImGui::showImGui) {
		//円形ロウゲージの位置
		ImGui::SetNextWindowSize({ 300, 250 }, ImGuiCond_FirstUseEver);

		ImGui::Begin("playerHPUI");

		ImGui::SliderFloat("現在の割合(システム側)", &baseRadian,0.0f,360.f);
		ImGui::SliderFloat("レート", &rate,0.0f,1.0f);
		ImGui::Text("現在の割合(ゲージ側):%f", circleGauge.baseRadian);
		ImGui::DragFloat3("位置", &circleGauge.baseTrans.position.x);
		ImGui::DragFloat("角度", &circleGauge.angle ,0.01f);
		ImGui::DragFloat("angleCompensNum", &angleCompensNum, 0.1f);

		if (player) {
			ImGui::DragFloat("playerRangeY", &playerRangeY, 0.1f);
		}

		if (ImGui::Button("セーブ"))
		{
			Parameter::Begin("playerHPUI");
			Parameter::Save("angleCompensNum", angleCompensNum);
			Parameter::Save("レート", rate);
			Parameter::Save("角度", circleGauge.angle);
			Parameter::End();
		}

		ImGui::End();
	}
}

void PlayerHpUI::Draw()
{
	circleGaugeBack.Draw();
	circleGaugeDam.Draw();
	circleGauge.Draw();
	
	circleGaugeFrame.Draw();

	InstantDrawer::DrawGraph(
		circleGaugeFrame.baseTrans.position.x,
		circleGaugeFrame.baseTrans.position.y,
		circleGaugeFrame.baseTrans.scale.x,
		circleGaugeFrame.baseTrans.scale.y,
		0,
		TextureManager::Load("./Resources/UI/hpText.png", "hpText"));
}

void PlayerHpUI::SetPlayer(Player* player_)
{
	player = player_;
}
