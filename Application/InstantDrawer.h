#pragma once
#include "Sprite.h"
#include "Color.h"
#include <list>
#include "BillboardImage.h"

/*! InstantDrawer
	DxLibライクな描画関数を利用できるクラス
*/

class InstantSprite : public Sprite
{
public:
	bool isUse = false;
};

class InstantBillboard : public BillboardImage
{
public:
	bool isUse = false;
};

class InstantDrawer
{
public:
	enum class Anchor
	{
		LEFT,
		CENTER,
		RIGHT,
		UP,
		DOWN,
	};

	/// <summary>
	/// 初期化
	/// </summary>
	static void DrawInit();

	
	/// <summary>
	/// 2Dで画像を描画する機能
	/// </summary>
	/// <param name="x">座標(X)</param>
	/// <param name="y">座標(Y)</param>
	/// <param name="sizerateX">描画倍率(X)</param>
	/// <param name="sizerateY">描画倍率(Y)</param>
	/// <param name="handle">テクスチャハンドル</param>
	/// <param name="anchor"></param>
	static void DrawGraph(
		const float& x, const float& y,
		float sizerateX,float sizerateY,float angle,
		const std::string& handle, const Color& color = { 1,1,1,1 },
		const Anchor& anchor = Anchor::CENTER);

	/// <summary>
	/// ビルボードで描画する
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="handle"></param>
	static void DrawGraph3D(const Vector3& pos, float width, float height, const std::string& handle,const Color& color = {1,1,1,1});

	/// <summary>
	/// 全更新
	/// </summary>
	static void AllUpdate();

	/// <summary>
	/// 全描画
	/// </summary>
	static void AllDraw2D();

	//事前にメモリを確保
	static void PreCreate();

private:
	static InstantDrawer* Get() {
		static InstantDrawer instance;
		return &instance;
	}

	bool mCreated = false;
	//この描画クラスで1画面に同時に描画できる最大数
	static const int SP_MAX = 100;
	std::list<InstantSprite> sSprites;
	static const int BILL_MAX = 100;
	std::list<InstantBillboard> sBills;
};