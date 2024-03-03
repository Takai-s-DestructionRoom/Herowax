/*
* @file Transform.h
* @brief いつもの3変換を提供するクラス
*/

#pragma once
#include "Matrix4.h"
#include "Vector3.h"

struct TransformBuffer
{
	Matrix4 matrix;
};

class Transform
{
public:
	Transform* parent = nullptr;
	Vector3 position = { 0, 0, 0 };
	Vector3 rotation = { 0, 0, 0 };
	Vector3 scale = { 1, 1, 1 };

	Matrix4 matrix;

	//pos(0, 0, 0), rot(0, 0, 0), scale(1, 1, 1)のTransformを生成
	Transform() {
		UpdateMatrix();
	}

	Transform(Vector3 position, Vector3 rotation, Vector3 scale) : position(position), rotation(rotation), scale(scale) {
		UpdateMatrix();
	}

	//メンバ変数の3情報でワールド座標変換行列を生成する
	void UpdateMatrix();

	/// <summary>
	/// 定数バッファへ転送
	/// </summary>
	/// <param name="target">対象のバッファへのポインタ</param>
	void Transfer(TransformBuffer* target);
};

