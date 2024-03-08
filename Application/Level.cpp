#include "Level.h"
#include "SpawnerManager.h"
#include "EnemyManager.h"
#include "RInput.h"
#include "Camera.h"

Level::Level()
{
}

void Level::Load()
{
	LevelLoader::Get()->Load("./Level/test.json", "test");
}

void Level::Reset()
{
	spawnerManager->Init();
	tower.Init();
	EnemyManager::GetInstance()->Init();
	EnemyManager::GetInstance()->SetTarget(&tower.obj);
}

void Level::Reload()
{
	LevelLoader::Get()->Load(nowLevel->mPath, nowLevel->mHandle);
	Extract(nowLevel->mHandle);
}

void Level::Update()
{
	//Rキーで再読み込み
	if (RInput::GetInstance()->GetKeyDown(DIK_R))
	{
		Reload();
	}

	spawnerManager->Update();

	EnemyManager::GetInstance()->Update();

	ground.mTransform.UpdateMatrix();
	ground.TransferBuffer(Camera::sNowCamera->mViewProjection);
	
	tower.Update();
}

void Level::Draw()
{
	spawnerManager->Draw();
	EnemyManager::GetInstance()->Draw();
	ground.Draw();
	tower.Draw();
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
			ground = ModelObj(Model::Load("./Resources/Model/Ground/ground.obj", "Ground"));
			
			//座標を設定
			ground.mTransform.position = objectData->translation;
			ground.mTransform.scale = objectData->scaling;
			ground.mTransform.rotation = objectData->rotation;
		}
		if (objectData->setObjectName == "EnemySpawner")
		{
			spawnerManager->Create(objectData->translation,
				objectData->scaling,
				objectData->rotation);
		}
		if (objectData->setObjectName == "Tower")
		{
			tower.Init(); 
			//座標を設定
			tower.SetPos(objectData->translation);
			tower.SetScale(objectData->scaling);
			tower.SetRota(objectData->rotation);
		}
	}
}
