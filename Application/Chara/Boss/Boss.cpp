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

	for (size_t i = 0; i < parts.size(); i++)
	{
		parts[i].obj = PaintableModelObj(Model::Load("./Resources/Model/VicViper/VicViper.obj", "VicViper", true));
		//parts[i].mTransform.parent = &obj.mTransform;
		parts[i].obj.mTransform.scale = Vector3::ONE * 2.f;
	}

	parts[(size_t)PartsNum::LeftHand].oriPos = { -50.f,20.f,0.f };
	parts[(size_t)PartsNum::RightHand].oriPos = { 50.f,20.f,0.f };

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

	for (size_t i = 0; i < parts.size(); i++)
	{
		parts[i].ChangeDrawCollider(isDrawCollider);
		parts[i].Update();
	}

#pragma region ImGui
	ImGui::SetNextWindowSize({ 300, 250 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Boss", NULL, window_flags);

	ImGui::Text("1:待機\n2:左パンチ\n3:右パンチ");
	ImGui::Checkbox("オブジェクト描画", &isDrawObj);
	ImGui::Checkbox("当たり判定描画", &isDrawCollider);

	ImGui::Text("手のコライダー座標\nx:%f\ny:%f\nz:%f",
		parts[0].collider.pos.x, parts[0].collider.pos.y, parts[0].collider.pos.z);
	ImGui::Text("手のコライダーサイズ:%f", parts[0].collider.r);
	ImGui::Text("手のコライダー描画フラグ:%d", parts[0].GetIsDrawCollider());

	ImGui::End();
}

void Boss::Draw()
{
	if (isAlive)
	{
		if (isDrawObj) {
			obj.Draw();
			for (size_t i = 0; i < parts.size(); i++)
			{
				parts[i].Draw();
			}
		}

		DrawCollider();
		for (size_t i = 0; i < parts.size(); i++)
		{
			parts[i].DrawCollider();
		}

		ui.Draw();
	}
}

void Parts::Init()
{
}

void Parts::Update()
{
	colliderSize = 20.f;

	UpdateCollider();

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Parts::Draw()
{
	obj.Draw();
}
