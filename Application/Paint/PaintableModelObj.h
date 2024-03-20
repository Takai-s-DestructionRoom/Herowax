/*
* @file PaintableModelObj.h
* @brief ModelObjのテクスチャペイント簡単版
*/

#pragma once
#include <ModelObj.h>
#include <Sprite.h>
#include <ColPrimitive3D.h>
#include <RenderTarget.h>
#include "ColPrimitive3D.h"

struct PaintableInfo
{
	ColPrimitive3D::Triangle tri;
	int32_t hitMeshIndex = 0;
	int32_t hitIndex = 0;
	float closestDis = FLT_MAX;
	Vector3 closestPos = { 0, 0, 0 };
};

class PaintableModelObj : public ModelObj
{
public:
	PaintableModelObj() : ModelObj() {};
	PaintableModelObj(Model* model) : ModelObj(model) {};
	PaintableModelObj(ModelHandle handle) : ModelObj(ModelManager::Get(handle)) {};

	~PaintableModelObj();

	//テクスチャペイント用の準備をする
	//どっかで呼んでください
	void SetupPaint();

	/// <summary>
	/// 指定したワールド座標とポリゴンからテクスチャペイントを行う
	/// </summary>
	/// <param name="pos">塗る位置(ワールド座標系)</param>
	/// <param name="hitMeshIndex">塗るメッシュのインデックス</param>
	/// <param name="hitIndicesIndex">塗るポリゴンのインデックス(3つで1ポリゴンとして何個目か)</param>
	/// <param name="brush">塗る画像のハンドル</param>
	/// <param name="color">塗る色(画像に乗算するので元画像が白でないなら何かあれになる)</param>
	/// <param name="size">塗るサイズ(まだ安定してないかも)</param>
	/// <param name="matrix">カメラの持つ行列</param>
	/// <returns>塗れたかどうか</returns>
	bool Paint(Vector3 pos, int32_t hitMeshIndex, int32_t hitIndicesIndex, TextureHandle brush, Color color, Vector2 size, Matrix4 matrix);

	/// <summary>
	/// レイで当たり判定を行ってテクスチャペイントを行う
	/// </summary>
	/// <param name="ray">レイ</param>
	/// <param name="brush">塗る画像のハンドル</param>
	/// <param name="color">塗る色(画像に乗算するので元画像が白でないなら何かあれになる)</param>
	/// <param name="size">塗るサイズ(まだ安定してないかも)</param>
	/// <param name="matrix">カメラの持つ行列</param>
	/// <returns>塗れたかどうか</returns>
	bool Paint(ColPrimitive3D::Ray ray, TextureHandle brush, Color color, Vector2 size, Matrix4 matrix);

	bool GetInfo(ColPrimitive3D::Ray ray, PaintableInfo *info = nullptr);

	std::vector<ColPrimitive3D::Triangle> GetTriangle()const;

	//各データのバッファへの転送
	void TransferBuffer(ViewProjection viewprojection) override;

	//描画要求を取得する(Rendererへは投げない)
	std::vector<RenderOrder> GetRenderOrder() override;

	//描画要求をRendererへ
	void Draw(std::string stageID = "Opaque") override;

private:
	bool mSetuped = false;
	RootSignature* mRootSignature = nullptr;
	GraphicsPipeline* mPipeline = nullptr;
	struct Painter {
		bool isUsing = false;
		Sprite sprite;
	};
	std::vector<Painter> mPainters;
	std::vector<RenderTexture*> mRenderTargets;
};

