/*
* @file PaintableModelObj.h
* @brief ModelObjのテクスチャペイント簡単版
*/

#pragma once
#include <ModelObj.h>
#include <Sprite.h>
#include <ColPrimitive3D.h>
#include <RenderTarget.h>

struct PaintData {
	Vector3 ambient = { 1, 1, 1 };
	float pad = 0;
	Vector3 diffuse = { 1, 1, 1 };
	float pad2 = 0;
	Vector3 specular = { 1, 1, 1 };
	float pad3 = 0;
	Color color = { 1, 1, 1, 1 };
	float dissolveVal = 0; //ディゾルブのかけ具合
	float slide = 0; //歪みのずらしにかける値
};

class PaintableModelObj : public ModelObj
{
public:
	TextureHandle mPaintDissolveMapTex;
	SRConstBuffer<PaintData> mPaintDataBuff;

	PaintableModelObj() : ModelObj() {};
	PaintableModelObj(Model* model) : ModelObj(model) {};
	PaintableModelObj(ModelHandle handle) : ModelObj(ModelManager::Get(handle)) {};

	//各データのバッファへの転送
	void TransferBuffer(ViewProjection viewprojection) override;

	//描画要求を取得する(Rendererへは投げない)
	std::vector<RenderOrder> GetRenderOrder() override;

	//描画要求をRendererへ
	void Draw(std::string stageID = "Opaque") override;

private:
	static RootSignature* GetRootSig();
	static GraphicsPipeline* GetPipeline();
};

