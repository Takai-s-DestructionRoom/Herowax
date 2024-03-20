#include "Fire.h"
#include "Camera.h"
#include "Util.h"
#include "ParticleManager.h"

Fire::Fire(std::vector<Vector3> splinePoints_):
	GameObject(),timer(2.0f)
{
	splinePoints = splinePoints_;

	obj = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));
	obj.mTuneMaterial.mColor = Color::kFireOutside;

	targetCircle = ModelObj(Model::Load("./Resources/Model/targetMark/targetMark.obj", "targetMark", true));
}

void Fire::Init()
{
	timer.Start();
}

void Fire::Update()
{
	timer.Update();
	if (timer.GetEnd()) {
		isAlive = false;
	}

	UpdateCollider();

	obj.mTransform.position = Util::Spline(splinePoints,timer.GetTimeRate());

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);

	targetCircle.mTransform.position = splinePoints.back();
	targetCircle.mTransform.rotation.y += 0.02f;

	targetCircle.mTransform.UpdateMatrix();
	targetCircle.TransferBuffer(Camera::sNowCamera->mViewProjection);


	//燃えてるときパーティクル出す
	ParticleManager::GetInstance()->AddSimple(
		obj.mTransform.position, obj.mTransform.scale * 0.8f, 2, 0.4f,
		Color::kFireOutside, TextureManager::Load("./Resources/particle_simple.png"),
		2.f, 4.f, { -0.1f,0.1f,-0.1f }, { 0.1f,0.5f,0.1f },
		0.05f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f, 0.f, false, true);
	//中心の炎
	ParticleManager::GetInstance()->AddSimple(
		obj.mTransform.position, obj.mTransform.scale * 0.3f, 2, 0.2f,
		Color::kFireInside, TextureManager::Load("./Resources/particle_simple.png"),
		2.f, 4.f, { -0.1f,0.1f,-0.1f }, { 0.1f,0.4f,0.1f },
		0.01f, -Vector3::ONE * 0.1f, Vector3::ONE * 0.1f, 0.05f, 0.f, false, true);
}

void Fire::Draw()
{
	obj.Draw();

	targetCircle.Draw();
}
