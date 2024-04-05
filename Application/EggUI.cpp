#include "EggUI.h"
#include "Texture.h"
#include "InstantDrawer.h"
#include "RImGui.h"
#include "Parameter.h"

void EggUI::LoadResource()
{
	TextureManager::Load("Resources/egg.png", "eggUI");
}

void EggUI::Init()
{
	auto extract = Parameter::Extract("EggUI");
	position.x = Parameter::GetParam(extract, "座標X", Util::WIN_WIDTH / 2);
	position.y = Parameter::GetParam(extract, "座標Y", 100);
	size.x = Parameter::GetParam(extract, "サイズX", 0.25f);
	size.y = Parameter::GetParam(extract, "サイズY", 0.25f);
	shakePower = Parameter::GetParam(extract, "揺れの強さ", 10.f);
	shakeTimer.maxTime_ = Parameter::GetParam(extract, "揺れる時間", 1.f);
}

void EggUI::Update()
{ 
	//もとに戻す
	position -= shake;
	shake = { 0,0 };
	color = { 1,1,1,1 };

	//タイマー更新
	shakeTimer.Update();

	if (tower) {
		//-----色を変える処理------//
		float hpRate = tower->hp / tower->maxHP;
		////HPが2割以下もしくはHP1になったら
		//if (hpRate <= 0.2f || tower->hp == 1) {
		//	//色を変える
		//	color = { 1,0,0,1 };
		//}
		////4割以下なら
		//else if (hpRate <= 0.4f) {
		//	//色を変える
		//	color = { 1,0.4f,0.4f,1 };
		//}

		//レートをそのまま色に入れる
		hpRate = Util::Clamp(hpRate, 0.0f, 1.0f);
		color = { 1,hpRate,hpRate,1 };

		//------揺れる処理-----//
		if (oldHP != tower->hp) {
			shakeTimer.Start();
		}
		oldHP = (int32_t)tower->hp;
	}
	if (shakeTimer.GetRun()) {
		shake.x = Util::GetRand(0.f, shakePower);
		shake.y = Util::GetRand(0.f, shakePower);

		position += shake;
	}

	//ImGui::SetNextWindowSize({ 300, 200 });

	//ImGuiWindowFlags window_flags = 0;
	//window_flags |= ImGuiWindowFlags_NoResize;

	//ImGui::Begin("EggUI", NULL, window_flags);
	//if (tower) {
	//	ImGui::Text("HP %f", tower->hp);
	//}
	//ImGui::SliderFloat("座標X", &position.x,0,Util::WIN_WIDTH);
	//ImGui::SliderFloat("座標Y", &position.y,0,Util::WIN_HEIGHT);
	//ImGui::SliderFloat2("サイズ", &size.x,0,1.f);
	//ImGui::InputFloat("揺れの強さ", &shakePower,1.0f);
	//ImGui::InputFloat("揺れる時間", &shakeTimer.maxTime_,1.0f);

	//if (ImGui::Button("セーブ")) {
	//	Parameter::Begin("EggUI");
	//	Parameter::Save("座標X", position.x);
	//	Parameter::Save("座標Y", position.y);
	//	Parameter::Save("サイズX", size.x);
	//	Parameter::Save("サイズY", size.y);
	//	Parameter::Save("揺れの強さ", shakePower);
	//	Parameter::Save("揺れる時間", shakeTimer.maxTime_);
	//	Parameter::End();
	//}

	//ImGui::End();
}

void EggUI::Draw()
{
	InstantDrawer::DrawGraph(position.x, position.y,
		size.x, size.y, 0, "eggUI",color);
}

void EggUI::SetTower(Tower* tower_)
{
	tower = tower_;
}
