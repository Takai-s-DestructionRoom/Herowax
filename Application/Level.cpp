#include "Level.h"
#include "SpawnerManager.h"
#include "EnemyManager.h"
#include "RInput.h"
#include "Camera.h"
#include "Util.h"
#include "PaintUtil.h"

Level::Level()
{
}

void Level::Load()
{
	LevelLoader::Get()->Load("./Level/test.json", "test");
	Model::Load("./Resources/Model/Cube.obj", "Cube", true);
}

void Level::Reset()
{
	spawnerManager->Init();
	tower.Init();
	EnemyManager::GetInstance()->Init();
	EnemyManager::GetInstance()->SetTarget(&tower.obj);
	objects.clear();
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

	for (auto& obj : objects)
	{
		obj.mTransform.UpdateMatrix();
		obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
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

	for (auto& obj : objects)
	{
		obj.Draw();
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
			ground = PaintableModelObj(Model::Load("./Resources/Model/Ground/ground.obj", "Ground"));
			
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
		//後ろに_Objがついているものであれば適用
		//書式として[ハンドル名]+[_Obj]になっていないとエラーとなる
		if (Util::ContainString(objectData->setObjectName, "_Obj"))
		{
			std::vector<std::string> objHandle = Util::StringSplit(objectData->setObjectName,"_");
			objects.emplace_back(PaintableModelObj(objHandle[0]));

			objects.back().mTransform.position = objectData->translation;
			objects.back().mTransform.scale = objectData->scaling;
			objects.back().mTransform.rotation = objectData->rotation;
			
			objects.back().SetupPaint();
		}
	}
}
