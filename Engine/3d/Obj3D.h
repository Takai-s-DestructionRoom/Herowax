/*
* @file Obj3D.h
* @brief 3Dオブジェクトの基底クラス
*/

#pragma once
#include "Transform.h"
#include "ViewProjection.h"

class Obj3D
{
protected:
	Obj3D* mParent = nullptr;

public:
	virtual ~Obj3D() {}

	Transform mTransform;

	void SetParent(Obj3D* parent) {
		mParent = parent;
		mTransform.parent = &parent->mTransform;
	}

	const Obj3D* GetParent() {
		return mParent;
	}

	//各データのバッファへの転送
	virtual void TransferBuffer(ViewProjection viewprojection) {}

	//描画要求をRendererへ
	virtual void Draw() {}

	//描画要求をRendererへ
	virtual void Draw(std::string stageID = "") {}

	//描画用のコマンドをまとめてコマンドリストに積む
	virtual void DrawCommands() {}
};

