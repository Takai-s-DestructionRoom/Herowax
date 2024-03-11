/*
* @file BillboardImage.h
* @brief ビルボードを3D空間上に描画するクラス
*/

#pragma once
#include "Obj3D.h"
#include "Texture.h"
#include "Vector2.h"
#include "Image3D.h"

class BillboardImage : public Obj3D
{
public:
	Image3D mImage;
	bool mUseBillboardY = false;

	BillboardImage() {};
	BillboardImage(TextureHandle texture, Vector2 size = {1, 1})
	{
		Init(texture, size);
	}

	//初期化処理
	void Init(TextureHandle texture, Vector2 size);

	void Update(const ViewProjection& vp);

	//各データのバッファへの転送
	void TransferBuffer(ViewProjection viewprojection) override;

	//描画要求を取得する(Rendererへは投げない)
	std::vector<RenderOrder> GetRenderOrder() override;

	//描画要求をRendererへ
	void Draw(std::string stageID = "");
};

