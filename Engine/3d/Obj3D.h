/*
* @file Obj3D.h
* @brief 3Dオブジェクトの基底クラス
*/

#pragma once
#include "Transform.h"
#include "ViewProjection.h"
#include <RenderOrder.h>

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

	//描画要求を取得する(Rendererへは投げない)
	virtual std::vector<RenderOrder> GetRenderOrder() { return std::vector<RenderOrder>(); }

	//描画要求をRendererへ
	virtual void Draw(std::string stageID = "") {}
};

