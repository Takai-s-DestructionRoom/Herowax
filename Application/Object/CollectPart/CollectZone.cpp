#include "CollectZone.h"
#include "Camera.h"
#include "InstantDrawer.h"
#include "Parameter.h"

void CollectZone::Init()
{
	obj = PaintableModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube"));

	obj.mTuneMaterial.mColor = { 1,0,0,0.7f };
	obj.mTuneMaterial.mAmbient = Vector3(1, 1, 1) * 100.f;
	obj.mTuneMaterial.mDiffuse = Vector3::ZERO;
	obj.mTuneMaterial.mSpecular = Vector3::ZERO;

	anvpos = { Util::WIN_WIDTH / 2,Util::WIN_HEIGHT / 2 };
	hampos = { Util::WIN_WIDTH / 2 + 100.f,Util::WIN_HEIGHT / 2 - 100.f };

	gatheredParts.clear();
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

	colorTimer.RoopReverse();
	createTimer.Update();
	hammerTimer.Update();

	if (hammerTimer.GetNowEnd()) {
		hammerTimer.Start();
	}

	hamRot = Easing::InQuad(0, -90, hammerTimer.GetTimeRate());

	isCreate = createTimer.GetRun();

	obj.mTransform.position = pos;
	obj.mTransform.scale = { scale.x,1,scale.y };

	aabbCol.pos = pos;
	aabbCol.size = { scale.x / 2,1,scale.y / 2 };

	brightColor.a = Easing::InQuad(0, 0.2f, colorTimer.GetTimeRate());

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

	UIDraw();
}

void CollectZone::Create(const CollectPart& collect)
{
	gatheredParts.emplace_back();
	gatheredParts.back() = std::make_unique<CollectPart>();
	//ゾーンの中心位置に配置(ちょい空中から落とす)
	Vector3 spawnPos = GetPos() + Vector3(0,10,0);
	gatheredParts.back()->SetPos(spawnPos);

	//モデルを収集したものと合わせる
	gatheredParts.back()->obj.mModel = collect.obj.mModel;
	gatheredParts.back()->Collect();
}

void CollectZone::UIDraw()
{
	if (isCreate) {
		InstantDrawer::DrawGraph(anvpos.x, anvpos.y, 0.5f, 0.5f, 0.0f, "anvil");
		InstantDrawer::DrawGraph(hampos.x, hampos.y, 0.5f, 0.5f, hamRot, "hammer");
	}
}

void CollectZone::GodModeCreate()
{
	createTimer.ReStart();
	hammerTimer.maxTime_ = createTimer.maxTime_ / 2;

	hammerTimer.ReStart();
}

void CollectZone::GodModeCreateStop()
{
	createTimer.Reset();
	hammerTimer.Reset();
}

bool CollectZone::GodModeCreateEnd()
{
	return createTimer.GetEnd();
}

int32_t CollectZone::GetPartsNum()
{
	return (int32_t)gatheredParts.size();
}
