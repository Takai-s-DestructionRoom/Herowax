#pragma once
#include "LevelLoader.h"
#include "ModelObj.h"
#include "SpawnerManager.h"
#include "ColPrimitive3D.h"
#include "Singleton.h"
#include "Ground.h"

//ステージの単位(ウェーブとかそういうやつ)
class Level : public Singleton
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

	static Level* Get();

public:
	LevelData* nowLevel = nullptr;

	std::list<ModelObj> objects;				//オブジェクトたち
	Ground* ground = nullptr;					//地面
	std::vector<ModelObj> wall;					//壁
	std::vector<ColPrimitive3D::Plane> wallCol;	//壁の当たり判定
	SpawnerManager* spawnerManager = SpawnerManager::GetInstance();	//スポナー
	
	float waveWaitTime = 1.0f;

	Vector2 moveLimitMax = {0,0};	//移動制限
	Vector2 moveLimitMin = {0,0};	//移動制限
};

