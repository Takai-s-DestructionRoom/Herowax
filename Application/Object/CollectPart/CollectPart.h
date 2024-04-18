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

	//出現時のバウンド
	void Bound();

	//ゾーン内でのバウンド
	void CollectBound();

	Vector3 collectPos;

private:
	Vector3 moveVec;	//このフレームの移動量
	Vector3 rotVec;
	Vector3 rotPlusVec;

	float gravity = 0.0f;		//重力の強さ
	float acceralation = 0.01f;	//毎フレーム加算する重力加速

	Vector3 boundVector; //跳ね返りベクトル

	float groundPos = 0.0f;

	float boundPower = 0.25f;

	Player* target = nullptr;
	bool isCollected = false;	//ゾーンに格納されているか

	bool isNewBound = true;

	int32_t collectNum = 0;
};

