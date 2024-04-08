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
	UpdateCollider();

	ui.Update(this);

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Boss::Draw()
{
	if (isAlive)
	{
		obj.Draw();

		if (isDrawCollider) {
			DrawCollider();
		}

		ui.Draw();
	}
}
