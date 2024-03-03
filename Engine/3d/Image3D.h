/*
* @file Image3D.h
* @brief 板ポリに一枚絵を貼って3D空間上に描画するクラス
*/

#pragma once
#include "Obj3D.h"
#include "SRConstBuffer.h"
#include "Material.h"
#include "Texture.h"
#include "Vector2.h"
#include "SRVertexBuffer.h"
#include "SRIndexBuffer.h"

class Image3D final : public Obj3D
{
private:
	TextureHandle mTexture;
	Vector2 mImageScale = { 1, 1 };
	Vector2 mSize = { 1, 1 };
	Vector2 mSrcPos = { 0, 0 };
	Vector2 mTexRect = { 0, 0 };
	Vector2 mAnchor = { 0.5f, 0.5f };
	bool mForceSize = false;
	bool mChangeFlag = false;

	void UpdateVertex();

public:
	enum class BlendMode : int32_t {
		OpaqueAlpha,
		OpaqueAdd,
		OpaqueSub,
		TransparentAlpha,
		TransparentAdd,
		TransparentSub,
		OverwriteAlpha,
		OverwriteAdd,
		OverwriteSub
	};

	BlendMode mBlendMode = BlendMode::OpaqueAlpha;

private:
	GraphicsPipeline GetPipeline();

public:
	Material mMaterial;
	//TransformはObj3Dにある

	SRVertexBuffer mVertBuff;
	SRIndexBuffer mIndexBuff;
	SRConstBuffer<MaterialBuffer> mMaterialBuff;
	SRConstBuffer<TransformBuffer> mTransformBuff;
	SRConstBuffer<ViewProjectionBuffer> mViewProjectionBuff;

	Image3D();

	Image3D(TextureHandle texture, Vector2 size = {1, 1}, bool forceSize = false);

	void SetTexture(TextureHandle texture);

	TextureHandle GetTexture() {
		return mTexture;
	}

	void SetSize(Vector2 size, bool forceSize = false);

	//アンカーポイントを変更する
	void SetAnchor(Vector2 anchor);

	/// <summary>
	/// テクスチャの切り出し位置を設定する
	/// つまりUV座標を加工する
	/// </summary>
	/// <param name="srcX">切り出し位置の左上X</param>
	/// <param name="srcY">切り出し位置の左上Y</param>
	/// <param name="width">切り出し範囲サイズX</param>
	/// <param name="height">切り出し範囲サイズY</param>
	void SetTexRect(int32_t srcX, int32_t srcY, int32_t width, int32_t height);

	/// <summary>
	/// テクスチャの切り出し位置を設定する
	/// つまりUV座標を加工する
	/// </summary>
	/// <param name="srcX">切り出し位置の左上X</param>
	/// <param name="srcY">切り出し位置の左上Y</param>
	/// <param name="width">切り出し範囲サイズX</param>
	/// <param name="height">切り出し範囲サイズY</param>
	void SetTexRect(float srcX, float srcY, float width, float height);

	Vector2 GetSize() {
		return mSize;
	}

	//初期化処理
	void Init();

	//各データのバッファへの転送
	void TransferBuffer(ViewProjection viewprojection) override;

	//描画要求をRendererへ
	void Draw(std::string stageID = "") override;
};

