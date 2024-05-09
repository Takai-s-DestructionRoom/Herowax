#include "PlayerUI.h"
#include "Player.h"
#include "InstantDrawer.h"
#include "Texture.h"
#include "ImGui.h"
#include "Renderer.h"
#include "Minimap.h"
#include "Camera.h"
#include "Parameter.h"

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

	waxCircleGauge.Init();
	//デフォルトと同じだけど明示的にしたいのでテクスチャ配置
	waxCircleGauge.sprite.SetTexture(TextureManager::Load("./Resources/circleGauge.png", "circleGauge"));

	waxCircleGaugeBack.Init();
	//テクスチャを背景用のモノに変更
	waxCircleGaugeBack.sprite.SetTexture(TextureManager::Load("./Resources/circleGaugeBack.png", "circleGaugeBack"));

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
	onePushSize *= 2.f;

	waxCircleGauge.radian = 360.f * (1.0f - (float)player->waxStock / player->maxWaxStock);
	waxCircleGaugeBack.radian = 0.0f;

	waxCircleGauge.sprite.mTransform.position = basePos;
	waxCircleGauge.sprite.mTransform.scale = baseScale;
	waxCircleGaugeBack.sprite.mTransform.position = basePos;
	waxCircleGaugeBack.sprite.mTransform.scale = baseScale;

	waxCircleGauge.Update();
	waxCircleGaugeBack.Update();
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

	waxCircleGaugeBack.Draw();
	waxCircleGauge.Draw();

	////描画範囲制限
	//Renderer::SetScissorRects({ Minimap::GetInstance()->rect });
	//minimapIconRange.Draw();
	//minimapIcon.Draw();

	////元の範囲に戻してあげる
	//Renderer::SetScissorRects({ Minimap::GetInstance()->defaultRect });
}
