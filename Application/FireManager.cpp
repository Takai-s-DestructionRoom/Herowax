#include "FireManager.h"
#include "Camera.h"

FireManager* FireManager::GetInstance()
{
	static FireManager instance;
	return &instance;
}

FireManager::FireManager() :
	fireRange(10.f), fireImpactTime(2.0f), predictionSpawnTimer(0.4f)
{
	predictionSpawnTimer.Start();

	targetCircle = ModelObj(Model::Load("./Resources/Model/targetMark/targetMark.obj", "targetMark"));
}

void FireManager::Init()
{
	fires.clear();
}

void FireManager::Update()
{
	splinePoints.clear();

	//始点:自身の座標(炎を出す味方を作るならソイツの座標に変更)
	//中点:終点座標のXZの半分を始点に足した値と、終点で指定した値の半分を足したY
	//終点:自身の正面方向に指定した値だけ進んだ場所
	Vector3 end = spawnObject->mTransform.position + throwVec * fireRange;
	end.y = 0;
	Vector3 middle = spawnObject->mTransform.position + throwVec * (fireRange / 2);
	middle.y += fireRange / 2;

	splinePoints.push_back(spawnObject->mTransform.position);
	splinePoints.push_back(middle);
	splinePoints.push_back(end);

	DrawersUpdate();

	//死んでるならリストから削除
	fires.remove_if([](Fire& fire) {
		return !fire.GetIsAlive();
		});
	
	for (auto& fire : fires)
	{
		fire.Update();
	}

	//ターゲットサークル
	targetCircle.mTransform.position = splinePoints.back();
	targetCircle.mTransform.rotation.y += 0.02f;

	targetCircle.mTransform.UpdateMatrix();
	targetCircle.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void FireManager::Draw()
{
	for (auto& fire : fires)
	{
		fire.Draw();
	}

	for (auto& drawer : predictionDrawers)
	{
		drawer.obj.Draw();
	}

	targetCircle.Draw();
}

void FireManager::SetThorwVec(const Vector3& throwVec_)
{
	throwVec = throwVec_;
}

void FireManager::SetTarget(ModelObj* target_)
{
	spawnObject = target_;
}

void FireManager::Create()
{
	//生成
	fires.emplace_back(splinePoints);
	fires.back().Init();
	fires.back().timer.maxTime_ = fireImpactTime;
}

void FireManager::DrawersUpdate()
{
	predictionSpawnTimer.Update();
	if (predictionSpawnTimer.GetEnd()) {
		predictionSpawnTimer.Start();
		predictionDrawers.emplace_back();
	}

	//死んでるならリストから削除
	predictionDrawers.remove_if([](FirePrediction& fire) {
		return !fire.isAlive;
		});

	for (auto& drawer : predictionDrawers)
	{
		drawer.Update(splinePoints);
	}
}

FirePrediction::FirePrediction() :
	isAlive(true), lifeTimer(10.0f)
{

	obj = ModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube", true));
	obj.mTuneMaterial.mColor = { 1.f,0.0f,0.0f,1.f };
	obj.mTransform.scale = { 0.25f,0.25f ,0.25f };
	lifeTimer.Start();
}

void FirePrediction::Update(std::vector<Vector3> splinePoints_)
{
	lifeTimer.Update();
	if (lifeTimer.GetEnd()) {
		isAlive = false;
	}

	obj.mTransform.position = Util::Spline(splinePoints_, lifeTimer.GetTimeRate());

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}
