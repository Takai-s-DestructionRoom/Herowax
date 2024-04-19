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

	bool isCreate = false;

	Easing::EaseTimer hammerTimer;

	float healPower = 1;	//回復量

private:
	std::vector<std::unique_ptr<CollectPart>> gatheredParts; //おいてあるパーツを保持

	Easing::EaseTimer colorTimer;

	Easing::EaseTimer createTimer;

	Vector2 hampos;
	Vector2 anvpos;
	float hamRot;

public:
	void Init()override;
	void Update()override;	//当たり判定更新など
	void Draw()override;//どの範囲がcollectZoneなのかを半透明オブジェクトで描画

	void Create(const CollectPart& collect);

	void UIDraw();

	void GodModeCreate();
	void GodModeCreateStop();

	bool GodModeCreateEnd();

	int32_t GetPartsNum();

	std::vector<std::unique_ptr<CollectPart>>* GetPartsData() {return &gatheredParts;};
};

