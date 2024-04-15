#include "CollectZone.h"
#include "Camera.h"

void CollectZone::Init()
{
	obj = PaintableModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube"));

	obj.mTuneMaterial.mColor = { 1,0,0,0.7f };
	obj.mTuneMaterial.mAmbient = Vector3::ONE * 100.f;
	obj.mTuneMaterial.mDiffuse = Vector3::ZERO;
	obj.mTuneMaterial.mSpecular = Vector3::ZERO;
}

void CollectZone::Update()
{
	for (auto itr = gatheredParts.begin(); itr != gatheredParts.end();)
	{
		//死んでたら殺す
		if (!(*itr)->isAlive) {
			itr = gatheredParts.erase(itr);
		}
		else {
			itr++;
		}
	}

	for (auto& part : gatheredParts)
	{
		part->Update();
	}

	timer.RoopReverse();

	obj.mTransform.position = pos;
	obj.mTransform.scale = { scale.x,1,scale.y };

	aabbCol.pos = pos;
	aabbCol.size = { scale.x / 2,1,scale.y / 2 };

	brightColor.a = Easing::InQuad(0, 0.2f, timer.GetTimeRate());

	obj.mTransform.UpdateMatrix();
	BrightTransferBuffer(Camera::sNowCamera->mViewProjection);
}

void CollectZone::Draw()
{
	for (auto& part : gatheredParts)
	{
		part->Draw();
	}
	BrightDraw("Transparent");
}

void CollectZone::Create(const Vector3& spawnPos)
{
	gatheredParts.emplace_back();
	gatheredParts.back() = std::make_unique<CollectPart>();
	gatheredParts.back()->SetPos(spawnPos);
}

int32_t CollectZone::GetPartsNum()
{
	return (int32_t)gatheredParts.size();
}
