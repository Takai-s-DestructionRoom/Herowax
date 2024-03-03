/*
* @file Cube.h
* @brief 直方体を3D空間上に描画するクラス
* 直方体なのにCubeとは
*/

#pragma once
#include "Obj3D.h"
#include "Image3D.h"
class Cube : public Obj3D
{
public:
	enum Direction {
		Front, Left, Back, Right, Top, Bottom
	};

	Image3D mFaces[6];
	Vector2 mSize = { 1, 1 };

	Cube() {};
	Cube(TextureHandle texture, Vector2 size = {1, 1}, bool forceSize = false);

	/// <summary>
	/// 指定した面のテクスチャをセットする
	/// </summary>
	/// <param name="texture">テクスチャハンドル</param>
	/// <param name="direction">面の向き</param>
	void SetTexture(TextureHandle texture, Direction direction);

	/// <summary>
	/// 全ての面のテクスチャをセットする
	/// </summary>
	/// <param name="texture"></param>
	void SetAllTexture(TextureHandle texture);


	void SetAllAlpha(float alpha);

	//面のTransformとかを更新する
	void UpdateFaces();

	//各データのバッファへの転送
	void TransferBuffer(ViewProjection viewprojection) override;

	//描画要求をRendererへ
	void Draw() override;

	//描画用のコマンドをまとめてコマンドリストに積む
	void DrawCommands() override;
};

