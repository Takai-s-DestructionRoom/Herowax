#include "PlayerHpUI.h"
#include "RImGui.h"
#include "Player.h"
#include "Parameter.h"

void PlayerHpUI::Init()
{
	circleGauge.Init();
	circleGaugeBack.Init();
	circleGaugeBack.SetTexture(TextureManager::Load("./Resources/circleGaugeBack.png", "circleGaugeBack"));

	std::map<std::string, std::string> extract = Parameter::Extract("playerHPUI");
	playerRangeXZ = Parameter::GetParam(extract,"ずらしXZ", playerRangeXZ);
	playerRangeY = Parameter::GetParam(extract,"ずらしY", playerRangeY);
	rate = Parameter::GetParam(extract,"レート", rate);
	circleGauge.angle = Parameter::GetParam(extract,"角度", circleGauge.angle);
	circleGauge.baseTrans.scale = Parameter::GetVector3Data(extract,"大きさ", circleGauge.baseTrans.scale);
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

	circleGauge.radian = baseMin + baseRadian * rate;
	circleGauge.Update();

	circleGaugeBack.radian = baseMin;
	circleGaugeBack.baseTrans = circleGauge.baseTrans;
	circleGaugeBack.angle = circleGauge.angle;
	circleGaugeBack.Update();

	if (RImGui::showImGui) {
		//円形ロウゲージの位置
		ImGui::SetNextWindowSize({ 300, 250 }, ImGuiCond_FirstUseEver);

		ImGui::Begin("playerHPUI");

		ImGui::SliderFloat("現在の割合(システム側)", &baseRadian,0.0f,360.f);
		ImGui::SliderFloat("レート", &rate,0.0f,1.0f);
		ImGui::Text("現在の割合(ゲージ側):%f", circleGauge.radian);
		ImGui::DragFloat3("位置", &circleGauge.baseTrans.position.x);
		ImGui::DragFloat("角度", &circleGauge.angle ,0.01f);
		ImGui::DragFloat3("大きさ", &circleGauge.baseTrans.scale.x,0.1f);
		
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
			Parameter::SaveVector3("大きさ", circleGauge.baseTrans.scale);
			Parameter::End();
		}

		ImGui::End();
	}
}

void PlayerHpUI::Draw()
{
	circleGauge.Draw();
	circleGaugeBack.Draw();
}

void PlayerHpUI::SetPlayer(Player* player_)
{
	player = player_;
}
