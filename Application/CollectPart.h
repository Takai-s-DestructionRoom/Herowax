#pragma once
#include "GameObject.h"
#include "ModelObj.h"

//プレイヤーが集めるパーツ　敵がドロップする
class CollectPart : public GameObject
{
public:
	CollectPart();
	void Init()override;
	void Update()override;
	void Draw()override;

	//運ばれる
	void Carrying(ModelObj* target_);

	//格納される
	void Collect();

private:
	Vector3 moveVec;
	float gravity = 0.01f;

	ModelObj* target = nullptr;
	bool isCollected = false;	//ゾーンに格納されているか
};

