#include "PlayerUI.h"
#include "Player.h"
#include "InstantDrawer.h"
#include "Texture.h"
#include "ImGui.h"
#include "Renderer.h"
#include "Minimap.h"
#include "Camera.h"
#include "Parameter.h"
#include "RImGui.h"

void PlayerUI::LoadResource()
{
	TextureManager::Load("./Resources/white2x2.png", "white2x2");
}

PlayerUI::PlayerUI()
{
	//色指定
	gaugeColor[(uint32_t)UIType::waxGauge] = Color::kWhite;
	gaugeColor[(uint32_t)UIType::fireGauge] = Color::kGreen;

	//サイズ指定
	for (uint32_t i = 0; i < (uint32_t)UIType::max; i++)
	{
		size[i] = { 5.f,0.2f };
		maxSize[i] = size[i] * 1.1f;
	}

	std::map<std::string, std::string> extract = Parameter::Extract("waxCircleGauge");
	
	basePos = Parameter::GetVector3Data(extract, "ゲージの位置", {0,0,0});
	baseScale = Parameter::GetVector3Data(extract,"ゲージの大きさ", { 1,1,1 });

	GaugeReset();
	GaugeAdd();

	/*iconSize = { 0.25f,0.25f };
	minimapIcon.SetTexture(TextureManager::Load("./Resources/minimap_icon.png", "minimapIcon"));
	minimapIcon.mMaterial.mColor = Color::kWhite;

	rangeSize = { 5.f,200.f };
	minimapIconRange.SetTexture(TextureManager::Load("./Resources/white2x2.png", "white2x2"));
	minimapIconRange.SetAnchor({ 0.5f,1.f });
	minimapIconRange.mMaterial.mColor = Color::kWhite;
	minimapIconRange.mMaterial.mColor.a = 0.3f;*/
}

void PlayerUI::Update(Player* player)
{
	//座標を合わせ続ける
	for (uint32_t i = 0; i < (uint32_t)UIType::max; i++)
	{
		position[i] = player->obj.mTransform.position;
		position[i].y += player->GetScale().y * 4.f + (float)(i) * 0.5f;
	}

	//ゲージの割合を指定されたサイズとかける
	size[(uint32_t)UIType::waxGauge].x =
		maxSize[(uint32_t)UIType::waxGauge].x * ((float)player->waxStock / (float)player->maxWaxStock);

	size[(uint32_t)UIType::fireGauge].x =
		maxSize[(uint32_t)UIType::fireGauge].x * player->hp / player->maxHP;

	////スクリーン座標を求める
	//screenPos = Minimap::GetInstance()->GetScreenPos(player->obj.mTransform.position);
	//minimapIcon.mTransform.position = { screenPos.x,screenPos.y,0.f };
	////回転適用
	//minimapIcon.mTransform.rotation.z =
	//	player->obj.mTransform.rotation.y;
	////決めたサイズに
	//iconSize = { player->obj.mTransform.scale.x * 0.2f,player->obj.mTransform.scale.z * 0.2f };
	//minimapIcon.mTransform.scale = Vector3(iconSize.x,iconSize.y,1.f) * Minimap::GetInstance()->iconSize;

	////更新忘れずに
	//minimapIcon.mTransform.UpdateMatrix();
	//minimapIcon.TransferBuffer();


	////スクリーン座標を求める
	//screenPosRange = Minimap::GetInstance()->GetScreenPos(player->obj.mTransform.position);
	//minimapIconRange.mTransform.position = { screenPosRange.x,screenPosRange.y,0.f };
	////回転適用
	//minimapIconRange.mTransform.rotation.z =
	//	player->obj.mTransform.rotation.y;
	////決めたサイズに
	//minimapIconRange.mTransform.scale =
	//	Vector3(player->waxCollectRange * 0.5f, player->waxCollectVertical * 0.5f, 1.f) * Minimap::GetInstance()->iconSize;

	////更新忘れずに
	//minimapIconRange.mTransform.UpdateMatrix();
	//minimapIconRange.TransferBuffer();

	//ボタンを一回押すごとに減る量
	float onePushSize = (36.f / (float)player->waxNum);
	
	//現在のロウの値を0.0f~1.0f~2.0f..で表したい
	baseRadian = ((onePushSize * (float)player->waxStock)) / 360.f;
	baseBackRadian = ((onePushSize * (float)player->maxWaxStock)) / 360.f;

	baseRadian = max(0.0f, baseRadian);
	baseBackRadian = max(0.0f, baseBackRadian);

	//ゲージが必要になったら増やす(1.0fを超えたら2本目を生成、2.0fを超えたら3本目を生成...を繰り返す)
	while (baseBackRadian > (float)waxCircleGauges.size())
	{
		GaugeAdd();
	};

	GaugeUpdate();

	if (RImGui::showImGui) {
		//円形ロウゲージの位置
		ImGui::SetNextWindowSize({ 300, 250 }, ImGuiCond_FirstUseEver);

		ImGui::Begin("CircleGauge");

		ImGui::Text("現在の割合:%f", baseRadian);
		ImGui::DragFloat3("位置", &basePos.x);
		ImGui::DragFloat3("大きさ", &baseScale.x);

		if (ImGui::Button("セーブ")) {
			Parameter::Begin("waxCircleGauge");
			Parameter::SaveVector3("ゲージの位置", basePos);
			Parameter::SaveVector3("ゲージの大きさ", baseScale);
			Parameter::End();
		}

		ImGui::End();
	}
}

void PlayerUI::Draw()
{
	for (uint32_t i = 0; i < (uint32_t)UIType::max; i++)
	{
		Vector3 backPos = position[i];
		backPos.z += 0.01f;
		InstantDrawer::DrawGraph3D(backPos, maxSize[i].x, maxSize[i].y, "white2x2", Color::kBlack);
		InstantDrawer::DrawGraph3D(position[i], size[i].x, size[i].y, "white2x2", gaugeColor[i]);
	}

	//描画は小さいものを後に描画(描画順を後ろから前に回していく)
	GaugeDraw();

	////描画範囲制限
	//Renderer::SetScissorRects({ Minimap::GetInstance()->rect });
	//minimapIconRange.Draw();
	//minimapIcon.Draw();

	////元の範囲に戻してあげる
	//Renderer::SetScissorRects({ Minimap::GetInstance()->defaultRect });
}

void PlayerUI::GaugeReset()
{
	waxCircleGauges.clear();
	waxCircleGaugeBacks.clear();
}

void PlayerUI::GaugeAdd()
{
	waxCircleGauges.emplace_back();
	waxCircleGauges.back().Init();
	//デフォルトと同じだけど明示的にしたいのでテクスチャ配置
	waxCircleGauges.back().sprite.SetTexture(TextureManager::Load("./Resources/circleGauge.png", "circleGauge"));
	waxCircleGauges.back().radian = 360.0f;
	
	waxCircleGaugeBacks.emplace_back();
	waxCircleGaugeBacks.back().Init();
	//デフォルトと同じだけど明示的にしたいのでテクスチャ配置
	waxCircleGaugeBacks.back().sprite.SetTexture(TextureManager::Load("./Resources/circleGaugeBack.png", "circleGaugeBack"));
	waxCircleGaugeBacks.back().radian = 360.0f;

}

void PlayerUI::GaugeUpdate()
{
	int32_t i = 0;//ここは1ゲージ目もこっちにしたら0にする
	for (auto& gaugeOnce : waxCircleGauges)
	{
		float calRad = (baseRadian - i);//後ろの1.0fは変数化
		gaugeOnce.radian = (360.f * calRad);

		if (baseRadian <= i + 1)
		{
			gaugeOnce.radian -= 360.f;
			gaugeOnce.radian = abs(gaugeOnce.radian);
		}
		else
		{
			gaugeOnce.radian = 0.0f;
		}

		gaugeOnce.sprite.mTransform.position = basePos;
		gaugeOnce.sprite.mTransform.scale = baseScale * (sizeCoeff + sizeCoeffPlus * i);
		gaugeOnce.Update();

		i++;
	}

	i = 0;
	for (auto& gaugeOnce : waxCircleGaugeBacks)
	{
		float calBackRad = (baseBackRadian - i);//後ろの1.0fは変数化
		gaugeOnce.radian = (360.f * calBackRad);

		if (baseBackRadian <= i + 1)
		{
			gaugeOnce.radian -= 360.f;
			gaugeOnce.radian = abs(gaugeOnce.radian);
		}
		else
		{
			gaugeOnce.radian = 0.0f;
		}

		gaugeOnce.sprite.mTransform.position = basePos;
		gaugeOnce.sprite.mTransform.scale = baseScale * (sizeCoeff + sizeCoeffPlus * i);
		gaugeOnce.Update();
		i++;
	}
}

void PlayerUI::GaugeDraw()
{	
	for (int32_t i = (int32_t)waxCircleGauges.size() - 1;
		i >= 0;
		i--)
	{
		if (baseBackRadian >= i) {
			waxCircleGaugeBacks[i].Draw();
			waxCircleGauges[i].Draw();
		}
	}
}
