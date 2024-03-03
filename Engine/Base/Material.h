/*
* @file Material.h
* @brief マテリアル情報のクラス
*/

#pragma once
#include "Color.h"
#include "Vector3.h"
#include <string>
#include "Texture.h"

struct MaterialBuffer {
	Color color;
	Vector3 ambient;
	float pad = 0;
	Vector3 diffuse;
	float pad2 = 0;
	Vector3 specular;
};

class Material
{
public:
	std::string mName; //おなまえドットコム
	TextureHandle mTexture; //テクスチャ
	Color mColor = {1, 1, 1, 1}; //色(RGBA)
	Vector3 mAmbient = {1, 1, 1}; //環境光
	Vector3 mDiffuse = {0, 0, 0}; //拡散反射光
	Vector3 mSpecular = {0, 0, 0}; //鏡面反射光

	Material() {}
	Material(bool allOne) {
		if (allOne) {
			mDiffuse = { 1, 1, 1 };
			mSpecular = { 1, 1, 1 };
		}
	}
	bool operator==(const Material& o) const;
	bool operator!=(const Material& o) const;

	/// <summary>
	/// 定数バッファへ転送
	/// </summary>
	/// <param name="target">対象のバッファへのポインタ</param>
	void Transfer(MaterialBuffer* target);
};
