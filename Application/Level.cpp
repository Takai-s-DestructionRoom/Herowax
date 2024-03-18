#include "Level.h"
#include "SpawnerManager.h"
#include "EnemyManager.h"
#include "RInput.h"
#include "Camera.h"
#include "Util.h"
#include "PaintUtil.h"
#include "RImGui.h"

Level::Level()
{
}

void Level::Load()
{
	LevelLoader::Get()->Load("./Level/test.json", "test");
	Model::Load("./Resources/Model/Cube.obj", "Cube", true);
	Model::Load("./Resources/Model/Slope.obj", "Slope", true);
}

void Level::Reset()
{
	spawnerManager->Init();
	EnemyManager::GetInstance()->Init();
	EnemyManager::GetInstance()->SetTarget(&GetTower()->obj);
	objects.clear();
}

void Level::Reload()
{
	LevelLoader::Get()->Load(nowLevel->mPath, nowLevel->mHandle);
	Extract(nowLevel->mHandle);
}

Tower* Level::GetTower()
{
	//タワーが見つかっていなかったら
	if (tower == nullptr) {
		//全検索して
		for (auto& gameObj : objects)
		{
			//タワーがあれば
			if (gameObj->GetObjectName() == "Tower") {
				//ポインタに入れる
				tower = static_cast<Tower*>(gameObj.get());
				break;
			}
		}
		//ここまでいって見つからないならぬるぽが返ってしまうのでassert
		assert(0 + "Towerがnullptrです");
	}

	//返す
	return tower;
}

Ground* Level::GetGround()
{
	//タワーが見つかっていなかったら
	if (ground == nullptr) {
		//全検索して
		for (auto& gameObj : objects)
		{
			//タワーがあれば
			if (gameObj->GetObjectName() == "Ground") {
				//ポインタに入れる
				ground = static_cast<Ground*>(gameObj.get());
				break;
			}
		}

		//ここまでいって見つからないならぬるぽが返ってしまうのでassert
		assert(0 + "Groundがnullptrです");
	}

	//返す
	return ground;
}

void Level::Update()
{
	//Rキーで再読み込み
	if (RInput::GetInstance()->GetKeyDown(DIK_R))
	{
		Reload();
	}

	for (auto& gameObj : objects)
	{
		gameObj->Update();
	}

	spawnerManager->Update();

#pragma region ImGui
	ImGui::SetNextWindowSize({ 350, 180 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("レベルデータ");

	bool oldView = isViewCol;
	ImGui::Checkbox("地面、卵、地形の当たり判定描画", &isViewCol);
	if (oldView != isViewCol) {
		for (auto& gameObj : objects)
		{
			gameObj->SetIsViewCol(isViewCol);
		}
	}

	if (ImGui::Button("再読み込み")) {
		Reload();
	}

	ImGui::End();

#pragma endregion
}

void Level::Draw()
{
	spawnerManager->Draw();
	
	for (auto& obj : objects)
	{
		obj->Draw();
	}
}

void Level::Extract(const std::string& handle)
{
	Reset();

	nowLevel = LevelLoader::Get()->GetData(handle);

	for (auto objectData = nowLevel->mObjects.begin(); 
		objectData != nowLevel->mObjects.end(); objectData++)
	{
		if (objectData->setObjectName == "Ground")
		{
			GameObject* loadObj = SetUpObject<Ground>(*objectData);
			//後呼びしないとモデルがないのでここで毎回書く
			loadObj->obj.SetupPaint();
		}
		if (objectData->setObjectName == "EnemySpawner")
		{
			spawnerManager->Create(objectData->translation,
				objectData->scaling,
				objectData->rotation);
		}
		if (objectData->setObjectName == "Tower")
		{
			GameObject* loadObj = SetUpObject<Tower>(*objectData);
			//HP初期化
			loadObj->Init();
			//後呼びしないとモデルがないのでここで毎回書く
			loadObj->obj.SetupPaint();
		}
		//後ろに_Objがついているものであれば適用
		//書式として[ハンドル名]+[_Obj]になっていないとエラーとなる
		if (Util::ContainString(objectData->setObjectName, "_Obj"))
		{
			GameObject* loadObj = SetUpObject<Terrain>(*objectData);
			std::vector<std::string> objHandle = Util::StringSplit(objectData->setObjectName,"_");
			loadObj->obj.mModel = ModelManager::Get(objHandle[0]);
			//後呼びしないとモデルがないのでここで毎回書く
			loadObj->obj.SetupPaint();
		}
	}
}
