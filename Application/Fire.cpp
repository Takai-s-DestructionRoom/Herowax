#include "Fire.h"
#include "Camera.h"
#include "Util.h"

Fire::Fire(std::vector<Vector3> splinePoints_):
	GameObject(),timer(2.0f)
{
	splinePoints = splinePoints_;

	obj = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));
	obj.mTuneMaterial.mColor = { 0.9f,0.1f,0.1f,1.f };

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

	obj.mTransform.position = Util::Spline(splinePoints,timer.GetTimeRate());

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);

	targetCircle.mTransform.position = splinePoints.back();
	targetCircle.mTransform.rotation.y += 0.02f;

	targetCircle.mTransform.UpdateMatrix();
	targetCircle.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void Fire::Draw()
{
	obj.Draw();

	targetCircle.Draw();
}
