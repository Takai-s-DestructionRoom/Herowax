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
	EnemyManager::GetInstance()->Init();
	objects.clear();
	wall.clear();
}

void Level::Reload()
{
	LevelLoader::Get()->Load(nowLevel->mPath, nowLevel->mHandle);
	Extract(nowLevel->mHandle);
}

Level* Level::Get()
{
	static Level instance;
	return &instance;
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

	for (auto& tempWall : Level::Get()->wall)
	{
		tempWall.mTransform.UpdateMatrix();
		tempWall.TransferBuffer(Camera::sNowCamera->mViewProjection);
	}
}

void Level::Draw()
{
	spawnerManager->Draw();
	EnemyManager::GetInstance()->Draw();
	ground.Draw();

	for (auto& tempWall : Level::Get()->wall)
	{
		tempWall.Draw();
	}

	for (auto& obj : objects)
	{
		obj.Draw();
	}
}

void Level::Extract(const std::string& handle)
{
	Reset();

	nowLevel = LevelLoader::Get()->GetData(handle);

	std::vector<float> saveStartTiming;

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
		if (objectData->setObjectName == "Enemy")
		{
			EnemyManager::GetInstance()->CreateEnemy<Enemy>(
				objectData->translation,
				objectData->scaling,
				objectData->rotation,
				objectData->behaviorOrder);
		}
		if (objectData->setObjectName == "EnemySpawner")
		{
			spawnerManager->Create(objectData->translation,
				objectData->scaling,
				objectData->rotation,
				objectData->spawnerOrder);

			SpawnOrderData temp = SpawnDataLoader::Load(objectData->spawnerOrder);
			saveStartTiming.push_back(temp.startTiming);
		}
		if (objectData->setObjectName == "Wall")
		{
			wall.emplace_back();
			wall.back() = ModelObj(Model::Load("./Resources/Model/Ami/Ami.obj", "Wall"));

			//座標を設定
			wall.back().mTransform.position = objectData->translation;
			wall.back().mTransform.scale = objectData->scaling / 2;
			wall.back().mTransform.rotation = objectData->rotation;

			//壁の当たり判定設定
			wallCol.emplace_back();

			Vector3 normal = wall.back().mTransform.position - Vector3(0, wall.back().mTransform.position.y, 0);
			wallCol.back().normal = normal.Normalize();
			wallCol.back().distance = Vector2(wall.back().mTransform.position.x, wall.back().mTransform.position.z).Length();
		}
		//後ろに_Objがついているものであれば適用
		//書式として[ハンドル名]+[_Obj]になっていないとエラーとなる
		if (Util::ContainString(objectData->setObjectName, "_Obj"))
		{
			std::vector<std::string> objHandle = Util::StringSplit(objectData->setObjectName, "_");
			objects.emplace_back(ModelObj(objHandle[0]));

			objects.back().mTransform.position = objectData->translation;
			objects.back().mTransform.scale = objectData->scaling;
			objects.back().mTransform.rotation = objectData->rotation;
		}
	}

	spawnerManager->Start(saveStartTiming);
}
