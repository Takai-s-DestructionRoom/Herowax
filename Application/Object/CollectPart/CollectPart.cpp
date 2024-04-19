#include "CollectPart.h"
#include "Util.h"
#include "Camera.h"
#include "Player.h"
#include "CollectPartManager.h"

CollectPart::CollectPart()
{
	Init();
}

void CollectPart::Init()
{
	int32_t rand = Util::GetRand(0, 2);
	if (rand == 0) {
		obj = PaintableModelObj("hexagonBolt");
	}
	if (rand == 1) {
		obj = PaintableModelObj("gear");
	}
	if (rand == 2) {
		obj = PaintableModelObj("plusScrew");
	}

	obj.mTransform.scale = { 3,3,3};
}

void CollectPart::Update()
{
	moveVec = {0, 0, 0};

	//重力加算
	gravity += acceralation;
	moveVec.y -= gravity;

	//取得されていないなら(後でステート管理に変える)
	if (target == nullptr && !isCollected) {
		
		Bound();
	}
	if (isCollected) {
		//地面座標を超えたら戻す
		CollectBound();
	}

	//移動適用
	obj.mTransform.position += moveVec;

	//回転適用
	obj.mTransform.rotation = rotVec;

	//ターゲットにつかまっているなら、そっちの座標に合うように上書きする
	if (target != nullptr)
	{
		//ターゲットの後ろにつく
		collectPos = target->GetPos();
		Vector3 frontVec = target->GetFrontVec();
		frontVec.y = 0;
		collectPos -= frontVec * (GetScale().x * GetScale().z) / 2 * (float)collectNum;
		obj.mTransform.position = collectPos;
	}

	UpdateCollider();

	obj.mTransform.UpdateMatrix();
	BrightTransferBuffer(Camera::sNowCamera->mViewProjection);
}

void CollectPart::Draw()
{
	BrightDraw();
	DrawCollider();
}

void CollectPart::Carrying(Player* target_)
{
	if (IsCarrying() || isCollected) return;

	//ターゲットの背中に背負われる
	target = target_;
	target->carryingParts.push_back(this);
	collectNum = (int32_t)target->carryingParts.size();
}

void CollectPart::Collect()
{
	target = nullptr;
	isCollected = true;
}

bool CollectPart::IsCarrying()
{
	return target != nullptr;
}

bool CollectPart::IsCollected()
{
	return (target == nullptr && isCollected);
}

void CollectPart::Bound()
{
	//地面座標にスライド
	if (obj.mTransform.position.y < groundPos + collider.r) {
		obj.mTransform.position.y = groundPos + collider.r;
		moveVec.y = 0;
		gravity = 0.0f;

		boundVector.y = 0;

		boundVector.y += boundPower;
		boundPower /= 4;
		if (boundPower < 0.01f)boundPower = 0.0f;
	
		moveVec.y += boundVector.y;
	}
}

void CollectPart::CollectBound()
{
	if (isNewBound) {
		boundVector.x = Util::GetRand(-0.5f, 0.5f);
		boundVector.z = Util::GetRand(-0.5f, 0.5f);

		//回転を作成
		rotPlusVec.x = Util::GetRand(-0.3f, 0.3f);
		rotPlusVec.z = Util::GetRand(-0.3f, 0.3f);

		isNewBound = false;
	}

	moveVec.x += boundVector.x;
	moveVec.z += boundVector.z;

	//地面座標にスライド
	if (obj.mTransform.position.y < groundPos + collider.r) {
		obj.mTransform.position.y = groundPos + collider.r;
		moveVec.y = 0;
		gravity = 0.0f;

		//縦の加算
		boundVector.y = 0;

		boundVector.y += boundPower;
		boundPower /= 4;
		if (boundPower < 0.01f)boundPower = 0.0f;

		moveVec.y += boundVector.y;

		//横の加算
		boundVector.x /= 1.5f;
		if (boundVector.x < 0.01f)boundVector.x = 0.0f;

		boundVector.z /= 1.5f;
		if (boundVector.z < 0.01f)boundVector.z = 0.0f;

		rotPlusVec.x /= 2;
		if (rotPlusVec.x < 0.01f)rotPlusVec.x = 0.0f;

		rotPlusVec.z /= 2;
		if (rotPlusVec.z < 0.01f)rotPlusVec.z = 0.0f;

	}

	rotVec += rotPlusVec;
}
