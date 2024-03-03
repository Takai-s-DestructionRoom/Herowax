/*
* @file ViewProjection.h
* @brief ビュープロジェクション行列を作るクラス
*/

#pragma once
#include "Matrix4.h"
#include "Util.h"
#include <Vector2.h>
#include "Vector3.h"

struct ViewProjectionBuffer
{
	Matrix4 matrix;
	Vector3 cameraPos;
};

class ViewProjection
{
public:
	enum class Type {
		Orthographic,
		Perspective
	};
	Type mType = Type::Perspective;
	Matrix4 mView;
	Matrix4 mProjection;
	Matrix4 mMatrix;
	Vector3 mEye = {0, 0, 0}; //視点座標
	Vector3 mTarget = {0, 0, 1}; //注視点座標
	Vector3 mUpVec = {0, 1, 0}; //上方向ベクトル

	Vector2 mOrthoSize = { 1, 1 };

	float mFov = Util::AngleToRadian(45); //画角(Rad)
	float mAspect = 1; //アスペクト比(横/縦)
	float mNearclip = 0.3f; //ニアクリップ距離
	float mFarclip = 1000; //ファークリップ距離

	//メンバ変数の情報で各行列を生成する
	void UpdateMatrix();

	/// <summary>
	/// 定数バッファへ転送
	/// </summary>
	/// <param name="target">対象のバッファへのポインタ</param>
	void Transfer(ViewProjectionBuffer* target);

	Vector2 WorldToScreen(Vector3 wPos);
	Vector2 WorldToScreen(Vector3 wPos, float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth);
};
