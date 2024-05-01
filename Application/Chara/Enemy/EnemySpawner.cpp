#include "EnemySpawner.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "Util.h"
#include "BombSolider.h"
#include "Tank.h"

EnemySpawner::EnemySpawner() : GameObject(),
	hp(0), maxHP(5.f), spawnNum(1),
	spawnRandomPos(5.f)
{
	obj = PaintableModelObj(Model::Load("./Resources/Model/stick.obj", "Stick", true));
	//色変更
	obj.mTuneMaterial.mColor = Color(0.3f, 0.3f, 0.3f, 1);
}

void EnemySpawner::Init()
{
	hp = maxHP;

	loadOrderFilename = "test";
	Load(loadOrderFilename);
}

void EnemySpawner::Init(const std::string& loadfile)
{
	hp = maxHP;

	loadOrderFilename = loadfile;
	Load(loadOrderFilename);
}

void EnemySpawner::Update()
{
	lifeTimer.Update();

	for (auto& order : orderData.orders)
	{
		//スポーン済みならスキップ
		if (order.spawnCompletion)continue;

		//スポーン時間を超えたら
		if (order.spawnTiming <= lifeTimer.nowTime_ && 
			lifeTimer.GetRun())
		{
			//敵の出現
			PopEnemy(obj.mTransform.position, order);

			order.spawnCompletion = true;
		}
	}

	//HP0になったら死ぬ 時間が終わっても死ぬ
	if (hp <= 0 || lifeTimer.GetEnd())
	{
		isAlive = false;
	}

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);

	UpdateCollider();
}

void EnemySpawner::Draw()
{
	//タイマーが開始していないなら描画しない
	if (!GetStarted())return;

	obj.Draw();
}

void EnemySpawner::LoadBehavior(const std::string& loadOrder)
{
	behaviorOrder = loadOrder;
}

bool EnemySpawner::GetStarted()
{
	return lifeTimer.GetStarted();
}

void EnemySpawner::Start()
{
	lifeTimer.Start();
}

void EnemySpawner::PopEnemy(const Vector3 position, const SpawnOrderOnce& order)
{
	for (int i = 0; i < order.spawnNum; i++)
	{
		Vector3 spawnPos = position;
		spawnPos.x += Util::GetRand(-spawnRandomPos, spawnRandomPos);
		spawnPos.z += Util::GetRand(-spawnRandomPos, spawnRandomPos);
		//ここで文字列に応じてクラスごとに敵を出現させる
		if (order.enemyClassName == "enemy") {
			EnemyManager::GetInstance()->CreateEnemy<Tank>(
				spawnPos,
				Vector3(1,1,1),
				Vector3(0,0,0),
				behaviorOrder,
				Tank::GetEnemyTag());
		}
		if (order.enemyClassName == "bombsolider") {
			EnemyManager::GetInstance()->CreateEnemy<BombSolider>(
				spawnPos,
				Vector3(1, 1, 1),
				Vector3(0, 0, 0), 
				behaviorOrder,
				BombSolider::GetEnemyTag());
		}
	}
}

void EnemySpawner::Load(const std::string fileName)
{
	SpawnOrderData temp = SpawnDataLoader::Load(fileName);
	lifeTimer.maxTime_ = temp.maxTime;
	startTiming = temp.startTiming;
	orderData.orders = temp.orders;
}
