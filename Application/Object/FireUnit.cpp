#include "FireUnit.h"
#include "FireManager.h"
#include "Camera.h"
#include "RInput.h"
#include "Quaternion.h"
#include "TimeManager.h"
#include "ParticleManager.h"
#include "ImGui.h"

FireUnit::FireUnit() :
	size(0.5f), fireGauge(0.f), maxFireGauge(5.f),
	fireStock(0), maxFireStock(5), floatingTimer(1.f), isFireStock(false),
	dist(2.f), rotTimer(2.5f), frameCount(0), fireAddFrame(3),
	accelAmount(0.01f)
{
	obj = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));
	obj.mTuneMaterial.mColor = Color::kFireOutside;

	offsetOrigin = { 2.f,1.f,-2.f };
	offset = offsetOrigin;
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
		fireObj.emplace_back();
		fireObj.back() = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));
		fireObj.back().mTuneMaterial.mColor = Color::kFireOutside;
		fireObj.back().mTransform.scale *= size * 0.5f;

		fireGauge = 0.f;
	}
	//行きすぎないように
	fireGauge = Util::Clamp(fireGauge, 0.f, maxFireGauge);

	//ストック性にしないなら無制限に出せる
	if (isFireStock == false)
	{
		fireStock = maxFireStock;
		if (fireObj.size() < (size_t)maxFireStock)
		{
			fireObj.emplace_back();
			fireObj.back() = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));
			fireObj.back().mTuneMaterial.mColor = Color::kFireOutside;
			fireObj.back().mTransform.scale *= size * 0.5f;
		}
	}

	Vector3 targetToObjVec = target.position - obj.mTransform.position;
	if (targetToObjVec.LengthSq() > offsetOrigin.LengthSq())
	{
		adulationAccel += accelAmount;
	}

	if (targetToObjVec.LengthSq() < offsetOrigin.LengthSq() * 1.5f)
	{
		adulationAccel -= accelAmount * 1.5f;
	}

	//無限に増減しないよう抑える
	adulationAccel = Util::Clamp(adulationAccel, 0.f, accelAmount * 60.f);

	obj.mTransform.position.x +=
		targetToObjVec.GetNormalize().x * adulationAccel;
	obj.mTransform.position.y +=
		targetToObjVec.GetNormalize().y * adulationAccel;
	obj.mTransform.position.z +=
		targetToObjVec.GetNormalize().z * adulationAccel;

	obj.mTransform.scale *= size;

	FireRotation();
	FireShot();

	frameCount++;
	if (frameCount >= fireAddFrame)
	{
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
	}

	//更新してからバッファに送る
	for (auto& fire : fireObj)
	{
		if (frameCount >= fireAddFrame)
		{
			//炎パーティクル出す
			ParticleManager::GetInstance()->AddSimple(
				fire.mTransform.position, fire.mTransform.scale * 0.3f, 1, 0.2f,
				Color::kFireOutside, TextureManager::Load("./Resources/fireEffect.png"),
				1.f, 2.f, { -0.1f,0.1f,-0.1f }, { 0.1f,0.5f,0.1f },
				0.05f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f, 0.f, false, true);
			//中心の炎
			ParticleManager::GetInstance()->AddSimple(
				fire.mTransform.position, fire.mTransform.scale * 0.2f, 1, 0.1f,
				Color::kFireInside, TextureManager::Load("./Resources/fireEffect.png"),
				1.f, 2.f, { -0.1f,0.1f,-0.1f }, { 0.1f,0.4f,0.1f },
				0.01f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f, 0.f, false, true);
		}

		fire.mTransform.UpdateMatrix();
		fire.TransferBuffer(Camera::sNowCamera->mViewProjection);
	}

	if (frameCount >= fireAddFrame)
	{
		frameCount = 0;
	}

	//更新してからバッファに送る
	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);


	ImGui::SetNextWindowSize({ 300, 150 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("FireUnit", NULL, window_flags);

	ImGui::Text("加速度:%f",adulationAccel);
	ImGui::Text("オフセット:%f,%f,%f",offset.x,offset.y,offset.z);
	ImGui::Text("座標:%f,%f,%f",obj.mTransform.position.x, obj.mTransform.position.y, obj.mTransform.position.z);

	ImGui::SliderFloat("加速度", &accelAmount, 0.f, 0.1f);
	ImGui::SliderFloat("プレイヤーとの距離X", &offsetOrigin.x, 0.f, 5.f);
	ImGui::SliderFloat("プレイヤーとの距離Z", &offsetOrigin.z, -5.f, 0.f);

	ImGui::End();
}

void FireUnit::Draw()
{
	obj.Draw();
	for (auto& fire : fireObj)
	{
		fire.Draw();
	}
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
			//情報をfiremanagerへ送信
			FireManager::GetInstance()->SetTarget(&fireObj.front());
			//放物線上に炎を投げる
			FireManager::GetInstance()->Create();
			//ストック数減らす
			fireStock--;
			fireObj.erase(fireObj.begin());
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

	offset = offsetOrigin;
	offset *= Quaternion::Euler(transform.rotation);
	//ふよふよさせる
	floatingTimer.RoopReverse();
	//offset.y = 1.f + Easing::InQuad(floatingTimer.GetTimeRate()) * 0.5f;

	target.position += offset;

	//正面ベクトルを取得
	frontVec = { 0,0,1 };
	frontVec.Normalize();
	frontVec *= Quaternion::Euler(transform.rotation);
	FireManager::GetInstance()->SetThorwVec(frontVec);
	FireManager::GetInstance()->SetEndReferencePoint(transform.position);

	//Y座標だけは別で代入しておく(ふよふよさせたいから)
	//obj.mTransform.position.y = target.position.y;
}

void FireUnit::FireRotation()
{
	rotTimer.Roop();

	//カメラから注視点へのベクトル
	Vector3 cameraVec = Camera::sNowCamera->mViewProjection.mTarget - Camera::sNowCamera->mViewProjection.mEye;
	//カメラの角度
	float cameraRad = atan2f(cameraVec.x, cameraVec.z);
	//なぜか前方ベクトルだと横向きになるので無理やり90度足して直す
	float frontVecRad = atan2f(frontVec.x, frontVec.y) + Util::AngleToRadian(90.f);

	for (size_t i = 0; i < fireObj.size(); i++)
	{
		//ぐるぐる角度出す
		float angle = Util::AngleToRadian((360.f / (float)maxFireStock) * (float)i);
		angle += Util::AngleToRadian(rotTimer.GetTimeRate() * 360.f);

		//ローカル座標に代入
		Vector3 plusPos;
		plusPos.x = dist * cosf(angle);
		plusPos.y = dist * sinf(angle);
		//カメラに対して平行になるように
		plusPos *= Matrix4::RotationY(cameraRad + frontVecRad);

		//ユニット基準にして代入
		std::next(fireObj.begin(), i)->mTransform.position = 
			obj.mTransform.position + plusPos;
	}
}
