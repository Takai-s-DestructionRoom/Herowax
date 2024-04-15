#include "CollectZone.h"
#include "Camera.h"

void CollectZone::Init()
{
	obj = PaintableModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube"));
}

void CollectZone::Update()
{
	timer.RoopReverse();

	obj.mTransform.position = pos;
	obj.mTransform.scale = { scale.x,1,scale.y };

	aabbCol.pos = pos;
	aabbCol.size = { scale.x / 2,1,scale.y / 2 };
	
	obj.mTuneMaterial.mColor = { 1,0,0,0.5f };
	obj.mTuneMaterial.mAmbient = Vector3::ONE * 100000.f;
	brightColor.a += Easing::InQuad(0, 0.2f, timer.GetTimeRate());
	
	obj.mTransform.UpdateMatrix();
	BrightTransferBuffer(Camera::sNowCamera->mViewProjection);
}

void CollectZone::Draw()
{
	BrightDraw("Transparent");
}

void CollectZone::Move(CollectPart* part)
{
	gatheredParts.push_back(part);
}
