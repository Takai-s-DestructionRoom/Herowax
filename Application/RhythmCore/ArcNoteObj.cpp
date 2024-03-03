#include "ArcNoteObj.h"
#include <Renderer.h>

void ArcNoteObj::TransferBuffer(ViewProjection viewprojection)
{
	mVertices.resize(9);

	mVertices[0].pos = mStartPos + Vector3(0, 1, 0);
	mVertices[1].pos = mStartPos + Vector3(-1, 0, 0);
	mVertices[2].pos = mStartPos + Vector3(0, -1, 0);
	mVertices[3].pos = mStartPos + Vector3(1, 0, 0);
	mVertices[4].pos = mGoalPos + Vector3(0, 1, 0);
	mVertices[5].pos = mGoalPos + Vector3(-1, 0, 0);
	mVertices[6].pos = mGoalPos + Vector3(0, -1, 0);
	mVertices[7].pos = mGoalPos + Vector3(1, 0, 0);
	mVertices[8].pos = mStartPos + Vector3(0, 0, -1);

	mVertices[0].uv = { 1.0f, 0.0f };
	mVertices[1].uv = { 0.0f, 1.0f };
	mVertices[2].uv = { 1.0f, 1.0f };
	mVertices[3].uv = { 0.0f, 1.0f };
	mVertices[4].uv = { 1.0f, 0.0f };
	mVertices[5].uv = { 0.0f, 0.0f };
	mVertices[6].uv = { 1.0f, 0.0f };
	mVertices[7].uv = { 0.0f, 0.0f };
	mVertices[8].uv = { 0.5f, 0.0f };

	std::vector<uint32_t> indices;

	indices.push_back(0);
	indices.push_back(8);
	indices.push_back(1);

	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(8);

	indices.push_back(1);
	indices.push_back(8);
	indices.push_back(2);

	indices.push_back(2);
	indices.push_back(8);
	indices.push_back(3);

	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(7);

	indices.push_back(7);
	indices.push_back(5);
	indices.push_back(6);

	indices.push_back(0);
	indices.push_back(4);
	indices.push_back(7);

	indices.push_back(0);
	indices.push_back(7);
	indices.push_back(3);

	indices.push_back(3);
	indices.push_back(7);
	indices.push_back(6);

	indices.push_back(3);
	indices.push_back(6);
	indices.push_back(2);

	indices.push_back(1);
	indices.push_back(6);
	indices.push_back(5);

	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(6);

	indices.push_back(0);
	indices.push_back(5);
	indices.push_back(4);

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(5);

	VertexPNU::CalcNormalVec(mVertices, indices);

	mVertBuff.Update(mVertices);
	mIndexBuff.Init(indices);
	mTransformBuff->matrix = Matrix4();
	mMaterial.Transfer(mMaterialBuff.Get());
}

void ArcNoteObj::Draw()
{
	std::vector<RootData> rootData = {
			{ TextureManager::Get("hogetest").mGpuHandle},
			{ RootDataType::SRBUFFER_CBV, mMaterialBuff.mBuff },
			{ RootDataType::SRBUFFER_CBV, mTransformBuff.mBuff },
			{ RootDataType::CAMERA },
			{ RootDataType::LIGHT }
	};


	std::string stage = "Opaque";
	if (mMaterial.mColor.a < 1.0f) stage = "Transparent";
	Renderer::DrawCall(stage, mVertBuff, mIndexBuff, static_cast<uint32_t>(42), rootData);
}
