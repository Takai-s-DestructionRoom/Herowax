#pragma once
#include "GameObject.h"
#include "ModelObj.h"

class Player;

//プレイヤーが集めるパーツ　敵がドロップする
class CollectPart : public GameObject
{
public:
	CollectPart();
	void Init()override;
	void Update()override;
	void Draw()override;

	//運ばれる
	void Carrying(Player* target_);

	//格納される
	void Collect();

	bool IsCarrying();

	bool IsCollected();

private:
	Vector3 moveVec;
	float gravity = 0.01f;

	Player* target = nullptr;
	bool isCollected = false;	//ゾーンに格納されているか
};

