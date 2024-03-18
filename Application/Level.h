#pragma once
#include "LevelLoader.h"
#include "Tower.h"
#include "SpawnerManager.h"
#include "Terrain.h"
#include "Ground.h"
#include <memory>

//ステージの単位(ウェーブとかそういうやつ)
class Level
{
public:
	Level();
	void Update();
	void Draw();

	//この中で読み込みたいファイルを書いておく(後々、特定の階層のファイルを読み込むように変更)
	void Load();

	//現在のステージを指定ハンドルのものに変更
	void Extract(const std::string& handle);
	
	//既存のステージにありそうなものを削除(手動追加)
	void Reset();

	//読み込みした後、ステージ変更
	void Reload();

	//タワーを返す
	Tower* GetTower();

	//地面を返す
	Ground* GetGround();

private:
	template <typename GameObjectName>
	GameObject* SetUpObject(const LevelData::ObjectData& objData) {
		objects.emplace_back();
		objects.back() = std::make_unique<GameObjectName>();
		objects.back()->obj.mTransform.position = objData.translation;
		objects.back()->obj.mTransform.scale = objData.scaling;
		objects.back()->obj.mTransform.rotation = objData.rotation;

		return objects.back().get();
	};

public:
	LevelData* nowLevel = nullptr;

	std::list<std::unique_ptr<GameObject>> objects;	//オブジェクトたち
	SpawnerManager* spawnerManager = SpawnerManager::GetInstance();	//スポナー
private:
	Tower *tower = nullptr;		//タワー
	Ground* ground = nullptr;	//地面

	bool isViewCol = false;
};