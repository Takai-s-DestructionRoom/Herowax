#include "Mtllib.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include "Util.h"
#include <PathUtil.h>

using namespace std;

Mtllib Mtllib::Load(std::string filepath)
{
    std::filesystem::path path = PathUtil::ConvertAbsolute(Util::ConvertStringToWString(filepath));

    Mtllib data;

    ifstream file;
    file.open(path.c_str());
    if (file.fail()) {
        assert(0);
    }

    Material loading;

    string line = "";
    while (getline(file, line)) {
        istringstream line_stream(line);

        string key;
        getline(line_stream, key, ' ');

        if (key[0] == '\t') {
            key.erase(key.begin());
        }

        if (key == "newmtl") { //マテリアル定義（お名前はここから）
            if (loading != Material()) {
                data.mMaterials.push_back(loading);
            }
            loading = Material();
            line_stream >> loading.mName;
        }

        if (key == "Ka") { //環境光
            line_stream >> loading.mAmbient.x;
            line_stream >> loading.mAmbient.y;
            line_stream >> loading.mAmbient.z;
        }

        if (key == "Kd") { //拡散反射光
            line_stream >> loading.mDiffuse.x;
            line_stream >> loading.mDiffuse.y;
            line_stream >> loading.mDiffuse.z;
        }

        if (key == "Ks") { //鏡面反射光
            line_stream >> loading.mSpecular.x;
            line_stream >> loading.mSpecular.y;
            line_stream >> loading.mSpecular.z;
        }

        if (key == "map_Kd") { //テクスチャ指定
            string texfile;
            line_stream >> texfile;
            loading.mTexture = TextureManager::Load(Util::ConvertWStringToString(PathUtil::ConvertAbsolute(Util::ConvertStringToWString(texfile), path).c_str()));
        }
    }

    data.mMaterials.push_back(loading);
    return data;
}
