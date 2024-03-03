/*
* @file ArcNoteObj.h
* @brief レーザーノートクラス
*/

#pragma once
#include "Obj3D.h"
#include <SRVertexBuffer.h>
#include <SRIndexBuffer.h>
#include <SRConstBuffer.h>
#include <Material.h>

class ArcNoteObj : public Obj3D
{
private:
	std::vector<VertexPNU> mVertices;

	SRVertexBuffer mVertBuff;
	SRIndexBuffer mIndexBuff;
	SRConstBuffer<TransformBuffer> mTransformBuff;
	SRConstBuffer<MaterialBuffer> mMaterialBuff;

public:
	Vector3 mStartPos;
	Vector3 mGoalPos;
	Material mMaterial;

	//各データのバッファへの転送
	void TransferBuffer(ViewProjection viewprojection) override;

	//描画要求をRendererへ
	void Draw() override;

	//描画用のコマンドをまとめてコマンドリストに積む
	void DrawCommands() override {}
};

