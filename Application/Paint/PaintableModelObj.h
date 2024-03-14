/*
* @file PaintableModelObj.h
* @brief ModelObjのテクスチャペイント簡単版
*/

#pragma once
#include <ModelObj.h>
#include <Sprite.h>
#include <ColPrimitive3D.h>
#include <RenderTarget.h>

class PaintableModelObj : public ModelObj
{
public:
	PaintableModelObj() : ModelObj() {};
	PaintableModelObj(Model* model) : ModelObj(model) {};
	PaintableModelObj(ModelHandle handle) : ModelObj(ModelManager::Get(handle)) {};

	void SetupPaint();

	bool Paint(ColPrimitive3D::Ray ray, TextureHandle brush, Color color, Vector2 size, Matrix4 matrix);

	//各データのバッファへの転送
	void TransferBuffer(ViewProjection viewprojection) override;

	//描画要求を取得する(Rendererへは投げない)
	std::vector<RenderOrder> GetRenderOrder() override;

	//描画要求をRendererへ
	void Draw(std::string stageID = "") override;

private:
	bool mSetuped = false;
	struct Painter {
		bool isUsing = false;
		Sprite sprite;
	};
	std::vector<Painter> mPainters;
	std::vector<RenderTexture*> mRenderTargets;
};

