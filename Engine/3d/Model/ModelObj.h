/*
* @file ModelObj.h
* @brief モデルデータを基に3Dオブジェクトを描画するクラス
*/

#pragma once
#include "Obj3D.h"
#include "Model.h"
#include "Material.h"
#include "Texture.h"
#include "RConstBuffer.h"
#include "SRConstBuffer.h"
#include <map>

class ModelObj : public Obj3D
{
public:
	Model* mModel = nullptr;
	Material mTuneMaterial = Material(true);

	std::unordered_map<std::string, SRConstBuffer<MaterialBuffer>> mMaterialBuffMap;
	SRConstBuffer<TransformBuffer> mTransformBuff;
	SRConstBuffer<ViewProjectionBuffer> mViewProjectionBuff;

	ModelObj() {};
	~ModelObj() {}
	ModelObj(Model* model) : mModel(model) {};
	ModelObj(ModelHandle handle) : mModel(ModelManager::Get(handle)) {};

	//各データのバッファへの転送
	void TransferBuffer(ViewProjection viewprojection) override;

	//描画要求を取得する(Rendererへは投げない)
	std::vector<RenderOrder> GetRenderOrder() override;

	//描画要求をRendererへ
	void Draw(std::string stageID = "") override;
};

