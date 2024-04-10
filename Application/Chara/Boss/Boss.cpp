#include "Boss.h"
#include "BossNormal.h"
#include "BossPunch.h"
#include "Camera.h"
#include "ParticleManager.h"
#include "RInput.h"
#include "ImGui.h"

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
		//parts[i].mTransform.parent = &obj.mTransform;
		parts[i].mTransform.scale = Vector3::ONE * 2.f;

		drawerObj[i] = PaintableModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));
	}

	handOriPos[(size_t)Parts::LeftHand] = { -50.f,20.f,0.f };
	handOriPos[(size_t)Parts::RightHand] = { 50.f,20.f,0.f };

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

	// モーションの変更(デバッグ用)
	if (RInput::GetInstance()->GetKeyDown(DIK_1))
	{
		state = std::make_unique<BossNormal>();
	}
	else if (RInput::GetInstance()->GetKeyDown(DIK_2))
	{
		state = std::make_unique<BossPunch>(true);
	}
	else if (RInput::GetInstance()->GetKeyDown(DIK_3))
	{
		state = std::make_unique<BossPunch>(false);
	}

	UpdateCollider();

	ui.Update(this);

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);

	for (size_t i = 0; i < (size_t)Parts::Max; i++)
	{
		parts[i].mTransform.UpdateMatrix();
		parts[i].TransferBuffer(Camera::sNowCamera->mViewProjection);

		collider[i].pos = parts[i].mTransform.position;
		collider[i].r = parts[i].mTransform.scale.x;
		
		drawerObj[i].mTransform.position = collider[i].pos;
		drawerObj[i].mTransform.scale = Vector3::ONE * collider[i].r * obj.mTransform.scale.x;
		drawerObj[i].mTuneMaterial.mColor.a = 0.5f;

		drawerObj[i].mTransform.UpdateMatrix();
		drawerObj[i].TransferBuffer(Camera::sNowCamera->mViewProjection);
	}

#pragma region ImGui
	ImGui::SetNextWindowSize({ 300, 250 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Boss", NULL, window_flags);

	ImGui::Text("1:待機\n2:左パンチ\n3:右パンチ");

	ImGui::End();
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
			for (size_t i = 0; i < drawerObj.size(); i++)
			{
				drawerObj[i].Draw();
			}
		}

		ui.Draw();
	}
}
