#include "EnemySpawner.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "Util.h"

EnemySpawner::EnemySpawner() : GameObject(),
	hp(0), maxHP(5), spawnInterval(3.f), spawnNum(1),
	spawnRandomPos(5.f)
{
	obj = ModelObj(Model::Load("./Resources/Model/stick.obj", "Stick", true));
	
	//色変更
	obj.mTuneMaterial.mColor = Color(0.3f, 0.3f, 0.3f, 1);
}

void EnemySpawner::Init()
{
	hp = maxHP;
	obj.mTransform.rotation.z = Util::AngleToRadian(90.f);

	spawnTimer.Start();
}

void EnemySpawner::Update()
{
	//敵の出現
	PopEnemy(obj.mTransform.position, spawnInterval);

	//HP0になったら死ぬ
	if (hp <= 0)
	{
		isAlive = false;
	}

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);

	UpdateCollider();
}

void EnemySpawner::Draw()
{
	obj.Draw();
}

void EnemySpawner::PopEnemy(const Vector3 position, float time)
{
	spawnTimer.Update();
	
	//時間設定
	spawnTimer.maxTime_ = time;
	
	//時間になったら出現
	if (spawnTimer.GetEnd()) {
		for (int i = 0; i < spawnNum; i++)
		{
			Vector3 spawnPos = position;
			spawnPos.x += Util::GetRand(-spawnRandomPos, spawnRandomPos);
			spawnPos.z += Util::GetRand(-spawnRandomPos, spawnRandomPos);
			EnemyManager::GetInstance()->CreateEnemy(spawnPos);
		}
		spawnTimer.Start();
	}
}
