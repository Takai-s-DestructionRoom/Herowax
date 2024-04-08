#include "Boss.h"
#include "Camera.h"
#include "ParticleManager.h"

Boss::Boss() : GameObject(),
moveSpeed(0.1f), hp(0), maxHP(10.f)
{
	state = std::make_unique<BossNormal>();
	nextState = nullptr;
	obj = PaintableModelObj(Model::Load("./Resources/Model/firewisp/firewisp.obj", "firewisp", true));
	obj.mTransform.scale = Vector3::ONE * 100.f;

	for (size_t i = 0; i < (size_t)Parts::Max; i++)
	{
		parts[i] = PaintableModelObj(Model::Load("./Resources/Model/VicViper/VicViper.obj", "VicViper", true));
		parts[i].mTransform.parent = &obj.mTransform;
		parts[i].mTransform.scale = Vector3::ONE * 0.02f;
	}

	handOriPos[(size_t)Parts::LeftHand] = { -0.5f,0.2f,0.f };
	handOriPos[(size_t)Parts::RightHand] = { 0.5f,0.2f,0.f };

	parts[(size_t)Parts::LeftHand].mTransform.position =
		handOriPos[(size_t)Parts::LeftHand];
	parts[(size_t)Parts::RightHand].mTransform.position =
		handOriPos[(size_t)Parts::RightHand];

	BossUI::LoadResource();
}

Boss::~Boss()
{
	//死んだときパーティクル出す
	ParticleManager::GetInstance()->AddSimple(
		obj.mTransform.position, obj.mTransform.scale * 0.5f, 20, 0.3f,
		Color::kWhite, "", 0.5f, 0.8f,
		{ -0.3f,-0.3f,-0.3f }, { 0.3f,0.3f,0.3f },
		0.05f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f);
}

void Boss::Init()
{
	hp = maxHP;
}

void Boss::Update()
{
	//各ステート時の固有処理
	state->Update(this);

	UpdateCollider();

	ui.Update(this);

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);

	for (size_t i = 0; i < (size_t)Parts::Max; i++)
	{
		parts[i].mTransform.UpdateMatrix();
		parts[i].TransferBuffer(Camera::sNowCamera->mViewProjection);
	}
}

void Boss::Draw()
{
	if (isAlive)
	{
		obj.Draw();
		for (size_t i = 0; i < (size_t)Parts::Max; i++)
		{
			parts[i].Draw();
		}

		if (isDrawCollider) {
			DrawCollider();
		}

		ui.Draw();
	}
}
