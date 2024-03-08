#include "LevelLoader.h"
#include <fstream>
#include <cassert>
#include <PathUtil.h>
#include "Util.h"

void LevelLoader::Load(const std::string& filename, const std::string& handle)
{
	const std::string fullpath = filename;

	std::filesystem::path path = PathUtil::ConvertAbsolute(
		Util::ConvertStringToWString(fullpath));

	//ファイルストリーム
	std::ifstream file;

	//ファイルを開く
	file.open(path.c_str());
	//ファイルオープン失敗をチェック
	if (file.fail())
	{
		assert(0);
	}
	
	//JSON文字列から解凍したデータ
	nlohmann::json deserialized;

	//解凍
	file >> deserialized;

	//正しいファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	//"name"を文字列として取得
	std::string name =
		deserialized["name"].get<std::string>();
	//正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);
	
	//レベルデータ格納用インスタンスを生成
	LevelData levelData;

	//"objects"の全オブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"])
	{
		ObjectLoad(levelData, object);
	}

	//生成が終わったらフラグを立てる
	levelData.isCreate = true;
	levelData.mPath = filename;
	levelData.mHandle = handle;

	LevelDatas[handle] = levelData;
}

LevelData* LevelLoader::GetData(const std::string& handle)
{
	if (LevelDatas[handle].isCreate == false)
	{
		return nullptr;
	}

	return &LevelDatas[handle];
}

void LevelLoader::ObjectLoad(LevelData& levelData,nlohmann::json& object)
{
	//"type"データがなければ不正とする
	assert(object.contains("type"));

	//種別を取得
	std::string type = object["type"].get<std::string>();

	//"MESH"なら
	if (type.compare("MESH") == 0)
	{
		//要素追加
		levelData.mObjects.emplace_back(LevelData::ObjectData{});
		//今追加した要素の参照を得る
		LevelData::ObjectData& objectData = levelData.mObjects.back();

		if (object.contains("setObject")) {
			objectData.setObjectName = object["setObject"];
		}

		// JSONデータを文字列に変換
		std::string jsonStr = object.dump();

		// 検索対象の文字列
		std::string searchString = "point_";
		// 文字列を検索
		if (jsonStr.find(searchString) != std::string::npos) {
			
		}

		//パラメータ読み込み
		nlohmann::json& transform = object["transform"];

		//座標
		//blenderとdirectXでは座標系が異なるため、ここで変換する(エクスポート時に変換するのが一般的ではある)
		objectData.translation.x = (float)transform["translation"][1];
		objectData.translation.y = (float)transform["translation"][2];
		objectData.translation.z = -1 * (float)transform["translation"][0];

		//回転
		
		objectData.rotation.x = -1 * Util::AngleToRadian((float)transform["rotation"][1]);
		objectData.rotation.y = -1 * (float)Util::AngleToRadian(transform["rotation"][2]);
		objectData.rotation.z = (float)Util::AngleToRadian(transform["rotation"][0]);

		//スケーリング
		objectData.scaling.x = (float)transform["scaling"][1];
		objectData.scaling.y = (float)transform["scaling"][2];
		objectData.scaling.z = (float)transform["scaling"][0];

		//collider
		nlohmann::json& collider = object["collider"];
		if (collider != nullptr)
		{
			objectData.collider.have = true;
			//中心点
			objectData.collider.center.x = (float)collider["center"][1];
			objectData.collider.center.y = (float)collider["center"][2];
			objectData.collider.center.z = (float)collider["center"][0];
			//大きさ
			objectData.collider.size.x = (float)collider["size"][1];
			objectData.collider.size.y = (float)collider["size"][2];
			objectData.collider.size.z = (float)collider["size"][0];
		}

		//子ノードがいるなら再帰
		//reverveしてメモリを確保しないと、メモリが移動する危険性あり
		if (object.contains("children"))
		{
			//reservenum++;
			//levelData->objects.reserve(reservenum);
			for (nlohmann::json& object_ : object["children"])
			{
				ObjectLoad(levelData, object_);
			}
		}
	}
}

