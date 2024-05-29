#include "PlayerHpUI.h"
#include "RImGui.h"
#include "Player.h"
#include "InstantDrawer.h"
#include "Parameter.h"

void PlayerHpUI::Init()
{
	circleGauge.Init();
	circleGauge.SetTexture(TextureManager::Load("./Resources/UI/hpGauge.png", "hpGauge"));
	circleGauge.SetColor(Color::kGreen);

	circleGaugeBack.Init();
	circleGaugeBack.SetTexture(TextureManager::Load("./Resources/UI/hpGauge.png", "hpGauge"));
	circleGaugeBack.SetColor({0.1f,0.1f, 0.1f, 1.0f});
	
	circleGaugeFrame.Init();
	circleGaugeFrame.SetTexture(TextureManager::Load("./Resources/UI/hpGaugeFrame.png", "hpGaugeFrame"));

	std::map<std::string, std::string> extract = Parameter::Extract("playerHPUI");
	playerRangeXZ = Parameter::GetParam(extract,"ずらしXZ", playerRangeXZ);
	playerRangeY = Parameter::GetParam(extract,"ずらしY", playerRangeY);
	rate = Parameter::GetParam(extract,"レート", rate);
	circleGauge.angle = Parameter::GetParam(extract,"角度", circleGauge.angle);
	circleGauge.baseTrans.scale = { 1,1,1 };
	circleGaugeBack.baseTrans.scale = { 1,1,1 };

	sizeFront.x = Parameter::GetParam(extract,"大きさ(本体)X", sizeFront.x);
	sizeFront.y = Parameter::GetParam(extract,"大きさ(本体)Y", sizeFront.y);
	sizeBack.x = Parameter::GetParam(extract,"大きさ(後ろ)X", sizeBack.x);
	sizeBack.y = Parameter::GetParam(extract,"大きさ(後ろ)Y", sizeBack.y);
}

void PlayerHpUI::Update()
{
	baseMax = 360.f * rate;
	baseMin = 360.f - baseMax;

	if (player) {
		baseRadian = (1.0f - (player->hp / player->maxHP)) * 360.f;

		circleGauge.baseTrans.position = player->obj.mTransform.position;

		Vector3 frontVec = player->obj.mTransform.position - Camera::sNowCamera->mViewProjection.mEye;
		frontVec.Normalize();
		circleGauge.baseTrans.position += frontVec * playerRangeXZ;
		circleGauge.baseTrans.position.y += playerRangeY;
	}

	circleGauge.SetBillboardSize(sizeFront);
	circleGaugeBack.SetBillboardSize(sizeFront);
	circleGaugeFrame.SetBillboardSize(sizeBack);

	circleGauge.baseRadian = baseMin + baseRadian * rate;
	circleGauge.Update();

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
		ImGui::DragFloat2("大きさ(本体)", &sizeFront.x,0.1f);
		ImGui::DragFloat2("大きさ(後ろ)", &sizeBack.x, 0.1f);

		if (player) {
			ImGui::Checkbox("プレイヤーの位置に配置", &checkBox);
			if (checkBox)
			{
				ImGui::DragFloat("playerRangeXZ", &playerRangeXZ, 0.1f);
				ImGui::DragFloat("playerRangeY", &playerRangeY, 0.1f);
			}
		}

		if (ImGui::Button("セーブ"))
		{
			Parameter::Begin("playerHPUI");
			Parameter::Save("ずらしXZ", playerRangeXZ);
			Parameter::Save("ずらしY", playerRangeY);
			Parameter::Save("レート", rate);
			Parameter::Save("角度", circleGauge.angle);
			Parameter::Save("大きさ(本体)X", sizeFront.x);
			Parameter::Save("大きさ(本体)Y", sizeFront.y);
			Parameter::Save("大きさ(後ろ)X", sizeBack.x);
			Parameter::Save("大きさ(後ろ)Y", sizeBack.y);
			Parameter::End();
		}

		ImGui::End();
	}
}

void PlayerHpUI::Draw()
{
	circleGaugeBack.Draw();
	circleGauge.Draw();
	
	circleGaugeFrame.Draw();

	InstantDrawer::DrawGraph3D(
		circleGaugeFrame.baseTrans.position,
		circleGaugeFrame.baseTrans.scale.x * 6.5f,
		circleGaugeFrame.baseTrans.scale.y * 6.5f,
		TextureManager::Load("./Resources/UI/hpText.png", "hpText"));
}

void PlayerHpUI::SetPlayer(Player* player_)
{
	player = player_;
}
