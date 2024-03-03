#include "ControllerScene.h"
#include "RImGui.h"
#include "RInput.h"
#include <Quaternion.h>

ControllerScene::ControllerScene()
{
	skydome = ModelObj(Model::Load("./Resources/Model/Skydome/Skydome.obj", "Skydome"));
	skydome.mTransform.scale = { 5, 5, 5 };
	skydome.mTransform.UpdateMatrix();

	model = ModelObj(Model::Load("./Resources/Model/VicViper/VicViper.obj", "Vic", true));

	camera.mViewProjection.mEye = { 0, 0, -10 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();
}

void ControllerScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
}

void ControllerScene::Update()
{
	//テストGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 150 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Model Control", NULL, window_flags);

		if (ImGui::Button("Reset")) {
			model.mTransform.position = { 0, 0, 0 };
			model.mTransform.rotation = { 0, 0, 0 };
		}

		ImGui::End();
	}

	//model.mTransform.rotation += { 0, Util::AngleToRadian(5) * RInput::GetLStick(true, true).x, 0 };

	mAngle.x += RInput::GetMouseMove().x / 10;
	mAngle.y -= RInput::GetMouseMove().y / 10;

	if (mAngle.x >= 360) {
		mAngle.x -= 360;
	}
	if (mAngle.x < 0) {
		mAngle.x += 360;
	}

	if (mAngle.y >= 90) {
		mAngle.y = 89.9f;
	}
	if (mAngle.y <= -90) {
		mAngle.y = -89.9f;
	}

	Vector3 vec = { 0, 0, 0 };
	vec.x += RInput::GetLStick(true, true).x;
	vec.z += RInput::GetLStick(true, true).y;
	vec *= Quaternion::AngleAxis({ 0, 1, 0 }, Util::AngleToRadian(mAngle.x));
	vec.Normalize();
	model.mTransform.position += vec;
	model.mTransform.UpdateMatrix();

	Vector3 targetPos = model.mTransform.position;

	Quaternion hoge = Quaternion::LookAt({ 1, 0, 0 });
	Quaternion cameraQ = Quaternion::AngleAxis({ 0, 1, 0 }, Util::AngleToRadian(mAngle.x));
	cameraQ *= Quaternion::AngleAxis(Vector3(1, 0, 0) * cameraQ, Util::AngleToRadian(mAngle.y));

	Quaternion nowRota = Quaternion::Euler(model.mTransform.rotation);
	nowRota.Normalize();
	if (vec.LengthSq() != 0) {
		if (nowRota.Dot(Quaternion::LookAt(vec)) < 0) {
			nowRota += (-Quaternion::LookAt(vec) - nowRota) / 5;
			nowRota.Normalize();
		}
		else {
			nowRota += (Quaternion::LookAt(vec) - nowRota) / 5;
			nowRota.Normalize();
		}
	}
	model.mTransform.rotation = nowRota.ToEuler();

	Vector3 cameraVec = { 0, 0, 1 };
	cameraVec *= cameraQ;
	cameraVec *= Quaternion::AngleAxis(Vector3(0, 1, 0).Cross(cameraVec), Util::AngleToRadian(20));
	cameraVec *= -20.0f;
	
	camera.mViewProjection.mEye = model.mTransform.position + cameraVec;
	camera.mViewProjection.mTarget = targetPos;
	camera.mViewProjection.UpdateMatrix();

	light.Update();

	skydome.TransferBuffer(camera.mViewProjection);
	model.TransferBuffer(camera.mViewProjection);
}

void ControllerScene::Draw()
{
	skydome.Draw();
	model.Draw();
}
