/*
* @file Mtllib.h
* @brief .obj形式で用いるマテリアル情報ファイルを読み込むクラス
*/

#pragma once
#include "Material.h"
#include <vector>

class Mtllib
{
public:
	std::vector<Material> mMaterials;

	static Mtllib Load(std::string filepath);
};

