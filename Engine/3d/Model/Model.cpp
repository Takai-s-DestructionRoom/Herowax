#include "Model.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include "Util.h"
#include "Mtllib.h"
#include "Matrix4.h"
#include <PathUtil.h>

using namespace std;

bool ModelMesh::operator==(const ModelMesh& o) const
{
    return mName == o.mName && mVertices == o.mVertices && mIndices == o.mIndices && mMaterial == o.mMaterial;
}

bool ModelMesh::operator!=(const ModelMesh& o) const
{
    return !(*this == o);
}

void ModelMesh::CalcSmoothedNormals()
{
    map<Vector3, vector<Vector3>> smoothData;

    for (VertexPNU v : mVertices) {
        smoothData[v.pos].push_back(v.normal);
    }

    for (VertexPNU& v : mVertices) {
        vector<Vector3> normals = smoothData[v.pos];

        Vector3 result;
        for (Vector3 n : normals) {
            result += n;
        }
        result /= static_cast<float>(normals.size());
        result.Normalize();

        v.normal = result;
    }
}

ModelHandle Model::Load(string filepath, ModelHandle handle, bool smooth)
{
    ModelManager* instance = ModelManager::GetInstance();
    Model model;

    std::filesystem::path path = PathUtil::ConvertAbsolute(Util::ConvertStringToWString(filepath));

    model.mPath = Util::ConvertWStringToString(path);

    ifstream file;
    file.open(path.c_str());
    if (file.fail()) {
        return "PreRegistedModel_Empty";
    }

    if (handle.empty()) {
        handle = "NoNameHandle_" + model.mPath;
    }

    std::unique_lock<std::recursive_mutex> lock(instance->mMutex);
    if (instance->mModelMap.find(handle) != instance->mModelMap.end()
        && instance->mModelMap[handle].mPath == Util::ConvertWStringToString(path)) {
        return handle;
    }
    lock.unlock();
    ModelMesh loading;

    vector<Material> materialList;
    vector<Vector3> vertPosList;
    vector<Vector2> vertTexcoordList;
    vector<Vector3> vertNormalList;

    string line = "";
    while (getline(file, line)) {
        istringstream line_stream(line);

        string key;
        getline(line_stream, key, ' ');

        if (key == "#") {
            continue;
        }

        if (key == "o") { //おなまえ
            if (loading != ModelMesh()) {
                if (smooth) loading.CalcSmoothedNormals();
                loading.mVertBuff.Init(loading.mVertices);
                loading.mIndexBuff.Init(loading.mIndices);
                model.mData.emplace_back(std::make_shared<ModelMesh>(loading));
            }

            loading = ModelMesh();
            line_stream >> loading.mName;
        }

        if (key == "v") { //頂点座標
            Vector3 pos;
            line_stream >> pos.x;
            line_stream >> pos.y;
            line_stream >> pos.z;

            vertPosList.emplace_back(pos.x, pos.y, pos.z);
        }

        if (key == "vt") { //頂点UV
            Vector2 texcoord;
            line_stream >> texcoord.x;
            line_stream >> texcoord.y;
            vertTexcoordList.emplace_back(texcoord.x, 1.0f - texcoord.y);
        }

        if (key == "vn") { //頂点法線
            Vector3 normal;

            line_stream >> normal.x;
            line_stream >> normal.y;
            line_stream >> normal.z;

            vertNormalList.emplace_back(normal.x, normal.y, normal.z);
        }

        if (key == "f") { //インデックス
            vector<string> indexs = Util::StringSplit(line_stream.str(), " ");

            vector<VertexPNU> _vertices;
            for (int32_t i = 1; i < indexs.size(); i++) {
                vector<string> indexText = Util::StringSplit(indexs[i], "/");

                bool ok = false;
                VertexPNU vertex;
                if (indexText.size() >= 1) {
                    int32_t index = atoi(indexText[0].c_str()) - 1;
                    if (index >= 0 && vertPosList.size() > index) {
                        vertex.pos = vertPosList[index];
                        ok = true;
                    }
                }
                if (indexText.size() >= 2) {
                    int32_t index = atoi(indexText[1].c_str()) - 1;
                    if (index >= 0 && vertTexcoordList.size() > index) {
                        vertex.uv = vertTexcoordList[index];
                    }
                }
                if (indexText.size() >= 3) {
                    int32_t index = atoi(indexText[2].c_str()) - 1;
                    if (index >= 0 && vertNormalList.size() > index) {
                        vertex.normal = vertNormalList[index];
                    }
                }

                if (ok) {
                    _vertices.emplace_back(vertex);
                } 
            }

            if (_vertices.size() >= 3) {
                for (int32_t i = 0; i < _vertices.size() - 2; i++) {
                    int32_t indexB = 1 + i;
                    int32_t indexC = 2 + i;

                    //めちゃくちゃ重いから一回廃止
                    /*size_t a = Util::IndexOf(model.vertexs, _vertices[0]);
                    size_t b = Util::IndexOf(model.vertexs, _vertices[indexB]);
                    size_t c = Util::IndexOf(model.vertexs, _vertices[indexC]);*/

                    size_t a = SIZE_T_MAX;
                    size_t b = SIZE_T_MAX;
                    size_t c = SIZE_T_MAX;

                    loading.mVertices.emplace_back(_vertices[0]);
                    a = loading.mVertices.size() - 1;
                    loading.mVertices.emplace_back(_vertices[indexB]);
                    b = loading.mVertices.size() - 1;
                    loading.mVertices.emplace_back(_vertices[indexC]);
                    c = loading.mVertices.size() - 1;

                    loading.mIndices.emplace_back((uint32_t)a);
                    loading.mIndices.emplace_back((uint32_t)b);
                    loading.mIndices.emplace_back((uint32_t)c);
                }
            }
        }

        if (key == "mtllib") { //マテリアルテンプレートライブラリ
            string mfilename;
            line_stream >> mfilename;
            Mtllib lib = Mtllib::Load(Util::ConvertWStringToString(PathUtil::ConvertAbsolute(mfilename, path).c_str()));

            for (Material& mat : lib.mMaterials) {
                materialList.push_back(mat);
            }
        }

        if (key == "usemtl") {
            if (loading.mMaterial != Material()) {
                string oldname = loading.mName;
                if (smooth) loading.CalcSmoothedNormals();
                loading.mVertBuff.Init(loading.mVertices);
                loading.mIndexBuff.Init(loading.mIndices);
                model.mData.emplace_back(std::make_shared<ModelMesh>(loading));
                loading = ModelMesh();
                loading.mName = oldname + "_ChangeMaterial";
            }
            string mtlName;
            line_stream >> mtlName;
            for (Material& mtl : materialList) {
                if (mtl.mName == mtlName) {
                    loading.mMaterial = mtl;
                }
            }
        }
    }

    if (smooth) loading.CalcSmoothedNormals();
    loading.mVertBuff.Init(loading.mVertices);
    loading.mIndexBuff.Init(loading.mIndices);
    model.mData.emplace_back(std::make_shared<ModelMesh>(loading));

    lock.lock();
    instance->mModelMap[handle] = model;
    lock.unlock();
    return handle;
}

ModelHandle Model::Register(ModelHandle handle, Model model)
{
    ModelManager* instance = ModelManager::GetInstance();
    std::lock_guard<std::recursive_mutex> lock(instance->mMutex);

    instance->mModelMap[handle] = model;
    return handle;
}

aiMatrix4x4 GetNodeAbsoluteTransform(const aiNode* node) {
    aiMatrix4x4 transform = node->mTransformation;

    if (node->mParent != nullptr) {
        aiMatrix4x4 pTransform = GetNodeAbsoluteTransform(node->mParent);
        transform = pTransform * transform;
    }

    return transform;
}

void NodeTraverse(const aiScene* scene, const aiNode* node, Model* model, const vector<Material>* materials) {
    for (uint32_t meshIndex = 0; meshIndex < node->mNumMeshes; meshIndex++) {
        ModelMesh data;

        aiMesh* mesh = scene->mMeshes[node->mMeshes[meshIndex]];

        //おなまえドットコム
        aiString _name = mesh->mName;
        data.mName = _name.C_Str();

        //トランスフォーム
        aiMatrix4x4 transform = GetNodeAbsoluteTransform(node);

        //頂点
        vector<VertexPNU> vertices;

        for (uint32_t posIndex = 0; posIndex < mesh->mNumVertices; posIndex++) {
            Vector3 pos = { 0, 0, 0 };
            Vector3 norm = { 0, 0, 0 };
            Vector2 uv = { 0, 0 };

            aiVector3D _pos = mesh->mVertices[posIndex];
            _pos *= transform;
            pos = Vector3(_pos.x, _pos.y, _pos.z);

            if (mesh->HasNormals()) {
                aiVector3D _norm = mesh->mNormals[posIndex];

                aiVector3D _matpos;
                aiVector3D _matrotAxis;
                ai_real _matrotang;
                aiVector3D _matsca;
                transform.Decompose(_matsca, _matrotAxis, _matrotang, _matpos);

                aiMatrix4x4 rotateMat;
                aiMatrix4x4::Rotation(_matrotang, _matrotAxis, rotateMat);
                _norm *= rotateMat;
                _norm.Normalize();
                norm = Vector3(_norm.x, _norm.y, _norm.z);
            }

            if (mesh->HasTextureCoords(0)) {
                aiVector3D _uv = mesh->mTextureCoords[0][posIndex];
                uv = Vector2(_uv.x, _uv.y);
            }

            VertexPNU vert;
            vert.pos = pos;
            vert.normal = norm;
            vert.uv = uv;
            vertices.push_back(vert);
        }

        for (VertexPNU& vert : vertices) {
            data.mVertices.push_back(vert);
        }

        //面(インデックス)
        for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
            aiFace face = mesh->mFaces[faceIndex];
            for (uint32_t vertIndex = 0; vertIndex < face.mNumIndices; vertIndex++) {
                data.mIndices.push_back(face.mIndices[vertIndex]);
            }
        }

        if (materials->size() > mesh->mMaterialIndex) {
            data.mMaterial = materials->at(mesh->mMaterialIndex);
        }

        //ボーン
        for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
            data.mBones.emplace_back();
            aiBone* bone = mesh->mBones[boneIndex];

            data.mBones.back().mName = bone->mName.C_Str();

            for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; weightIndex++) {
                aiVertexWeight weight = bone->mWeights[weightIndex];
                data.mBones.back().mVertexWeights.push_back({ weight.mVertexId, weight.mWeight });
            }

            Matrix4& offsetMatrix = data.mBones.back().mOffsetMatrix;
            offsetMatrix[0][0] = bone->mOffsetMatrix.a1;
            offsetMatrix[0][1] = bone->mOffsetMatrix.a2;
            offsetMatrix[0][2] = bone->mOffsetMatrix.a3;
            offsetMatrix[0][3] = bone->mOffsetMatrix.a4;
            offsetMatrix[1][0] = bone->mOffsetMatrix.b1;
            offsetMatrix[1][1] = bone->mOffsetMatrix.b2;
            offsetMatrix[1][2] = bone->mOffsetMatrix.b3;
            offsetMatrix[1][3] = bone->mOffsetMatrix.b4;
            offsetMatrix[2][0] = bone->mOffsetMatrix.c1;
            offsetMatrix[2][1] = bone->mOffsetMatrix.c2;
            offsetMatrix[2][2] = bone->mOffsetMatrix.c3;
            offsetMatrix[2][3] = bone->mOffsetMatrix.c4;
            offsetMatrix[3][0] = bone->mOffsetMatrix.d1;
            offsetMatrix[3][1] = bone->mOffsetMatrix.d2;
            offsetMatrix[3][2] = bone->mOffsetMatrix.d3;
            offsetMatrix[3][3] = bone->mOffsetMatrix.d4;
        }

        data.mVertBuff.Init(data.mVertices);
        data.mIndexBuff.Init(data.mIndices);
        model->mData.push_back(make_shared<ModelMesh>(data));
    }

    for (uint32_t childIndex = 0; childIndex < node->mNumChildren; childIndex++) {
        NodeTraverse(scene, node->mChildren[childIndex], model, materials);
    }
}

ModelHandle Model::LoadWithAIL(std::string directoryPath, std::string filename, ModelHandle handle)
{
    ModelManager* instance = ModelManager::GetInstance();
    Model model;

    if (directoryPath[directoryPath.size() - 1] != '/') {
        directoryPath += "/";
    }

    model.mPath = directoryPath + filename;

    if (handle.empty()) {
        handle = "UnnamedModelHandle_" + model.mPath;
    }

    std::unique_lock<std::recursive_mutex> lock(instance->mMutex);
    if (instance->mModelMap.find(handle) != instance->mModelMap.end()
        && instance->mModelMap[handle].mPath == (model.mPath)) {
        return handle;
    }
    lock.unlock();

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(model.mPath,
        aiProcess_ConvertToLeftHanded |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_GenSmoothNormals |
//        aiProcess_FixInfacingNormals |
        aiProcess_CalcTangentSpace
    );

    if (scene == nullptr) {
        OutputDebugStringA(importer.GetErrorString());
        return "";
    }

    model.mName = scene->mName.C_Str();

    //マテリアル
    vector<Material> materials;

    for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; materialIndex++) {
        Material material;
        aiMaterial* _material = scene->mMaterials[materialIndex];

        aiString str;
        _material->Get(AI_MATKEY_NAME, str);
        material.mName = str.C_Str();

        aiColor3D color;

        color = { 0, 0, 0 };
        _material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        material.mDiffuse = Vector3(color.r, color.g, color.b);
        
        color = { 0, 0, 0 };
        _material->Get(AI_MATKEY_COLOR_AMBIENT, color);
        material.mAmbient = Vector3(color.r, color.g, color.b);

        color = { 0, 0, 0 };
        _material->Get(AI_MATKEY_COLOR_SPECULAR, color);
        material.mSpecular = Vector3(color.r, color.g, color.b);

        float opacity = 0.0f;
        _material->Get(AI_MATKEY_OPACITY, opacity);
        material.mColor.a = opacity;

        if (_material->GetTextureCount(aiTextureType_DIFFUSE) >= 1) {
            //_material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
            _material->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), str);

            string path = str.C_Str();
            string texHandle = "";

            if (!material.mName.empty()) {
                texHandle = model.mPath + "_" + material.mName + "_Tex";
            }
            else {
                texHandle = "UnnamedMaterial_Tex";
            }

            material.mTexture = "";
            if (path.empty()) {
                material.mTexture = "";
            }
            else {
                const aiTexture* embedTex = scene->GetEmbeddedTexture(path.c_str());
                if (embedTex != nullptr) {
                    if (embedTex->mHeight == 0) {
                        material.mTexture = TextureManager::Load(embedTex->pcData, embedTex->mWidth, path, texHandle);
                    }
                    else {
                        vector<Color> data;
                        data.resize(embedTex->mWidth * embedTex->mHeight);

                        for (uint32_t i = 0; i < embedTex->mWidth * embedTex->mHeight; i++) {
                            aiTexel texel = embedTex->pcData[i];
                            data[i] = Color(texel.r, texel.g, texel.b, texel.a);
                        }

                        material.mTexture = TextureManager::Create(&data[0], embedTex->mWidth, embedTex->mHeight, path, texHandle);
                        
                    }
                }
                else {
                    if (path[0] == '*') {
                        string index = path.substr(1, path.size() - 1);
                        aiTexture* tex = scene->mTextures[atoi(index.c_str())];
                        
                        if (tex->mHeight == 0) {
                            material.mTexture = TextureManager::Load(tex->pcData, tex->mWidth, path, texHandle);
                        }
                        else {
                            vector<Color> data;
                            data.resize(tex->mWidth * tex->mHeight);

                            for (uint32_t i = 0; i < tex->mWidth * tex->mHeight; i++) {
                                aiTexel texel = tex->pcData[i];
                                //data[i] = Color(texel.r, texel.g, texel.b, texel.a);
                            }

                            material.mTexture = TextureManager::Create(&data[0], tex->mWidth, tex->mHeight, path, texHandle);
                        }
                    }
                    else {
                        material.mTexture = TextureManager::Load(directoryPath + path, texHandle);
                    }
                }
            }
        }

        materials.push_back(material);
    }

    //ルートノードからぜーんぶメッシュをぶち込んでモデルにする
    NodeTraverse(scene, scene->mRootNode, &model, &materials);

    lock.lock();
    instance->mModelMap[handle] = model;
    lock.unlock();
    return handle;
}

Model* ModelManager::Get(ModelHandle handle)
{
    ModelManager* instance = GetInstance();
    std::lock_guard<std::recursive_mutex> lock(instance->mMutex);

    if (instance->mModelMap.find(handle) == instance->mModelMap.end()) {
        return &instance->mModelMap["PreRegisteredModel_Empty"];
    }

    return &instance->mModelMap[handle];
}

void ModelManager::Init()
{
    Model model;
    ModelMesh data;

    data.mVertices.push_back(VertexPNU({ -1, -1,  1 }, { 0, 0, 0 }, { 0, 1 })); //0
    data.mVertices.push_back(VertexPNU({  1, -1,  1 }, { 0, 0, 0 }, { 1, 1 })); //1
    data.mVertices.push_back(VertexPNU({ -1, -1, -1 }, { 0, 0, 0 }, { 0, 1 })); //2
    data.mVertices.push_back(VertexPNU({  1, -1, -1 }, { 0, 0, 0 }, { 1, 1 })); //3
    data.mVertices.push_back(VertexPNU({  0,  1,  0 }, { 0, 0, 0 }, { 0.5f, 0.0f })); //4

    data.mIndices.push_back(2);
    data.mIndices.push_back(3);
    data.mIndices.push_back(1);
    data.mIndices.push_back(2);
    data.mIndices.push_back(1);
    data.mIndices.push_back(0);
    data.mIndices.push_back(2);
    data.mIndices.push_back(4);
    data.mIndices.push_back(3);
    data.mIndices.push_back(3);
    data.mIndices.push_back(4);
    data.mIndices.push_back(1);
    data.mIndices.push_back(1);
    data.mIndices.push_back(4);
    data.mIndices.push_back(0);
    data.mIndices.push_back(0);
    data.mIndices.push_back(4);
    data.mIndices.push_back(2);

    VertexPNU::CalcNormalVec(data.mVertices, data.mIndices);

    data.mVertBuff.Init(data.mVertices);
    data.mIndexBuff.Init(data.mIndices);
    model.mData.emplace_back(std::make_shared<ModelMesh>(data));
    mModelMap["PreRegisteredModel_Empty"] = model;
}
