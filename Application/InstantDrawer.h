#pragma once
#include "Sprite.h"
#include "Color.h"
#include <list>

/*! InstantDrawer
	DxLibライクな描画関数を利用できるクラス
*/

class InstantSprite : public Sprite
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
		const std::string& handle, const Anchor& anchor = Anchor::CENTER);

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

	//この描画クラスで1画面に同時に描画できる最大数
	static const int SP_MAX = 100;
	std::list<InstantSprite> sSprites;
};