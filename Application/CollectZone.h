#pragma once
#include "Vector3.h"
#include "Vector2.h"
#include "CollectPart.h"
#include <memory>
#include "GameObject.h"
#include "ColPrimitive3D.h"
#include "Easing.h"

//collectPartを集めるゾーン
class CollectZone : public GameObject
{
public:
	Vector3 pos;	//ゾーンの位置(y=地面座標の予定)
	Vector2 scale;	//ゾーンの幅(xとz方向)

	ColPrimitive3D::AABB aabbCol;

private:
	std::vector<CollectPart*> gatheredParts; //おいてあるパーツを保持

	Easing::EaseTimer timer;
public:
	void Init()override;
	void Update()override;	//当たり判定更新など
	void Draw()override;//どの範囲がcollectZoneなのかを半透明オブジェクトで描画

	void Move(CollectPart* part);
};

