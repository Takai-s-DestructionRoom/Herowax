#include "CollectPart.h"
#include "Util.h"
#include "Camera.h"

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
	moveVec.x = 0;
	moveVec.z = 0;

	//取得されていないなら(後でステート管理に変える)
	if (target == nullptr && !isCollected) {
		moveVec.y -= gravity;

		obj.mTransform.position += moveVec;

		//地面座標を超えたら戻す
		if (obj.mTransform.position.y < 0) {
			obj.mTransform.position.y = 0;
			moveVec.y = 0;
		}
	}
	//されているなら
	if (target != nullptr)
	{
		//一旦ターゲットの位置に合わせる
		//後で位置調整する
		obj.mTransform.position = target->mTransform.position;
	}
	if (isCollected) {
		//地面座標を超えたら戻す
		if (obj.mTransform.position.y < 0) {
			obj.mTransform.position.y = 0;
			moveVec.y = 0;
		}
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

void CollectPart::Carrying(ModelObj* target_)
{
	if (IsCarrying() || isCollected) return;

	//ターゲットの背中に背負われる
	target = target_;
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