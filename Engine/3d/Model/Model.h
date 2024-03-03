/*
* @file Model.h
* @brief モデルデータを持つクラス
*/

#pragma once
#include <string>
#include <vector>
#include <mutex>
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RConstBuffer.h"
#include "Material.h"
#include "Matrix4.h"

//assimp
#pragma warning(push)
#pragma warning(disable: 26451)
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>
#pragma warning(pop)

typedef std::string ModelHandle;

struct VertexWeight
{
	uint32_t vertID;
	float weight;
};

struct ModelBone
{
	std::string mName;
	std::vector<VertexWeight> mVertexWeights;
	Matrix4 mOffsetMatrix;
};

struct ModelMesh
{
	std::string mName;
	VertexBuffer mVertBuff;
	IndexBuffer mIndexBuff;

	std::vector<VertexPNU> mVertices;
	std::vector<uint32_t> mIndices;
	std::vector<ModelBone> mBones;
	Material mMaterial;

	void CalcSmoothedNormals();

	bool operator==(const ModelMesh& o) const;
	bool operator!=(const ModelMesh& o) const;
};

class Model
{
public:
	std::string mName;
	std::string mPath;
	std::vector<std::shared_ptr<ModelMesh>> mData;

	static ModelHandle Load(std::string filepath, ModelHandle handle = "", bool smooth = false);
	static ModelHandle Register(ModelHandle handle, Model model);

	//Assimp経由で読み込む
	static ModelHandle LoadWithAIL(std::string directoryPath, std::string filename, ModelHandle handle = "");
};

class ModelManager
{
public:
	ModelManager() {
		Init();
	}

	static ModelManager* GetInstance() {
		static ModelManager manager;
		return &manager;
	}

	static Model* Get(ModelHandle handle);

	std::map<ModelHandle, Model> mModelMap;
	std::recursive_mutex mMutex;

private:
	void Init();
};
