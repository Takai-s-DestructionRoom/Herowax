#pragma once
#include "Fire.h"
#include <vector>
#include "Vector3.h"
#include "Easing.h"

class FirePrediction 
{
public:
	bool isAlive;
	Easing::EaseTimer lifeTimer;

	ModelObj obj;

public:
	FirePrediction();
	void Update(std::vector<Vector3> splinePoints_);
};

class FireManager final
{
public:
	std::list<Fire> fires;	//炎ども

	//------------ 炎関連 ------------//
	float fireRange;				//炎の射出位置
	float fireImpactTime;			//炎が着弾するまでの時間

	Vector3 throwVec;				//投げる方向
	Vector3 endReferencePoint;		//着弾の基準点

	std::vector<Vector3> splinePoints;	//炎が通るスプライン挙動

	//------------ 描画関連 ------------//
	std::list<FirePrediction> predictionDrawers;	//放物線の予測線を描画
	ModelObj targetCircle;					//放物線の着地位置を描画
	Easing::EaseTimer predictionSpawnTimer;	//放物線の予測線の描画間隔

	ModelObj *spawnObject = nullptr;		//始点となるキャラクター

public:
	//シングルトンインスタンス取得
	static FireManager* GetInstance();

	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw();

	//外部から情報を設定しておくやつら
	void SetThorwVec(const Vector3& throwVec_);
	void SetTarget(ModelObj* target_);
	//着弾の基準点設定
	void SetEndReferencePoint(Vector3 point) { endReferencePoint = point; }

	//情報を元に生成
	void Create();

	//描画用のオブジェクトの更新
	//処理が煩雑化するので分化
	void DrawersUpdate();

private:
	//コンストラクタ
	FireManager(); 
	//コピー禁止
	FireManager(const FireManager&) = delete;
	//デストラクタ
	~FireManager() = default;
	//コピー禁止
	FireManager& operator=(const FireManager&) = delete;
};

