#include "FireUnit.h"
#include "FireManager.h"
#include "Camera.h"
#include "RInput.h"
#include "Quaternion.h"
#include "TimeManager.h"
#include "ParticleManager.h"

FireUnit::FireUnit():
	size(0.5f),fireGauge(0.f), maxFireGauge(5.f),
	fireStock(0), maxFireStock(5), floatingTimer(1.f),isFireStock(false)
{
	obj = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));
	obj.mTuneMaterial.mColor = Color::kFireOutside;

	offset = { 2.f,1.f,-1.f };
}

void FireUnit::Init()
{
	//最初から出せないの不便なので初期状態で半分はあげる
	fireStock = maxFireStock / 2;

	//情報をfiremanagerへ送信
	FireManager::GetInstance()->SetTarget(&obj);
}

void FireUnit::Update()
{
	//情報をfiremanagerへ送信
	FireManager::GetInstance()->SetTarget(&obj);
	
	//ゲージが溜まって、ストックも最大じゃないなら
	if (fireGauge >= maxFireGauge && fireStock < maxFireStock)
	{
		//ストック数を1増やしてゲージリセット
		fireStock++;
		fireGauge = 0;
	}
	//行きすぎないように
	fireGauge = Util::Clamp(fireGauge, 0.f, maxFireGauge);

	//ストック性にしないなら無制限に出せる
	if (isFireStock == false)
	{
		fireStock = maxFireStock;
	}

	//ふよふよさせる
	floatingTimer.RoopReverse();
	offset.y = 1.f + Easing::InQuad(floatingTimer.GetTimeRate()) * 0.5f;

	FireShot();

	//炎パーティクル出す
	ParticleManager::GetInstance()->AddSimple(
		obj.mTransform.position, obj.mTransform.scale * 0.5f, 1, 0.4f,
		Color::kFireOutside, TextureManager::Load("./Resources/fireEffect.png"),
		1.5f, 3.f, { -0.1f,0.1f,-0.1f }, { 0.1f,0.5f,0.1f },
		0.05f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f, 0.f, false, true);
	//中心の炎
	ParticleManager::GetInstance()->AddSimple(
		obj.mTransform.position, obj.mTransform.scale * 0.3f, 1, 0.2f,
		Color::kFireInside, TextureManager::Load("./Resources/fireEffect.png"),
		1.5f, 3.f, { -0.1f,0.1f,-0.1f }, { 0.1f,0.4f,0.1f },
		0.01f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f, 0.f, false, true);

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void FireUnit::Draw()
{
	obj.Draw();
}

void FireUnit::FireShot()
{
	//炎のストックあるときに
	if (fireStock > 0)
	{
		//押し込んだら
		if (RInput::GetPadButtonDown(XINPUT_GAMEPAD_RIGHT_THUMB) ||
			RInput::GetInstance()->GetKeyDown(DIK_F))
		{
			//放物線上に炎を投げる
			FireManager::GetInstance()->Create();
			//ストック数減らす
			fireStock--;
		}
	}
}

void FireUnit::FireGaugeCharge(float gauge)
{
	//最大数に達してないときだけ溜まる
	if (fireStock < maxFireStock)
	{
		fireGauge += gauge;
	}
}

void FireUnit::SetTransform(Transform transform)
{
	target = transform;
	target.position += offset;

	frontVec *= Quaternion::Euler(transform.rotation);
	FireManager::GetInstance()->SetThorwVec(frontVec);
	FireManager::GetInstance()->SetEndReferencePoint(transform.position);

	obj.mTransform = target;
	obj.mTransform.scale *= size;
}
