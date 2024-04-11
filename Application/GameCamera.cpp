#include "GameCamera.h"
#include "Vector2.h"
#include "RInput.h"
#include "Parameter.h"
#include "Quaternion.h"
#include "RImGui.h"

void GameCamera::Init()
{
	camera.mViewProjection.mEye = { 0, 0, -50 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();

	Camera::sNowCamera = &camera;

	std::map<std::string, std::string> extract = Parameter::Extract("Camera");
	cameraDist = Parameter::GetParam(extract, "カメラ距離", -20.f);
	cameraAngle.x = Parameter::GetParam(extract, "カメラアングルX", Util::AngleToRadian(20.f));
	cameraAngle.y = Parameter::GetParam(extract, "カメラアングルY", 0.f);
	cameraSpeed.x = Parameter::GetParam(extract, "カメラの移動速度X", 0.01f);
	cameraSpeed.y = Parameter::GetParam(extract, "カメラの移動速度Y", 0.003f);
	mmCameraDist = Parameter::GetParam(extract, "ミニマップ用カメラ距離", -250.f);
	cameraUpOffset = Parameter::GetParam(extract, "プレイヤーからのYのオフセット", cameraUpOffset);
}

void GameCamera::Update()
{
	Vector2 stick = RInput::GetInstance()->GetRStick(false, true);

	if (stick.LengthSq() > 0.0f) {
		if (abs(stick.x) > 0.3f) {
			cameraAngle.y += cameraSpeed.x * -stick.x;
		}
		if (abs(stick.y) > 0.3f) {
			cameraAngle.x += cameraSpeed.y * stick.y;
		}
	}
	cameraAngle.x = Util::Clamp(cameraAngle.x, 0.f, Util::AngleToRadian(89.f));

	Vector3 cameraVec = { 0, 0, 1 };
	//カメラアングル適応
	cameraVec *= Quaternion::AngleAxis(Vector3(1, 0, 0).Cross(cameraVec), cameraAngle.y);
	cameraVec *= Quaternion::AngleAxis(Vector3(0, 1, 0).Cross(cameraVec), cameraAngle.x);
	//カメラの距離適応
	cameraVec *= cameraDist;

	//プレイヤーと一定の距離を保って着いていく
	camera.mViewProjection.mEye = target->mTransform.position + cameraVec;
	//プレイヤーの方向いてくれる
	camera.mViewProjection.mTarget = target->mTransform.position;
	camera.mViewProjection.mTarget.y += cameraUpOffset;
	camera.mViewProjection.UpdateMatrix();

	ImGui::SetNextWindowSize({ 500, 200 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	// カメラ //
	ImGui::Begin("Camera", NULL, window_flags);

	ImGui::Text("スティック x: % f y : % f", stick.x, stick.y);
	ImGui::Text("座標:%f,%f,%f",
		camera.mViewProjection.mEye.x,
		camera.mViewProjection.mEye.y,
		camera.mViewProjection.mEye.z);
	ImGui::SliderFloat("カメラ距離:%f", &cameraDist, -500.f, 0.f);
	ImGui::DragFloat("プレイヤーからのYのオフセット:%f", &cameraUpOffset, 0.1f);
	ImGui::SliderAngle("カメラアングルX:%f", &cameraAngle.x);
	ImGui::SliderAngle("カメラアングルY:%f", &cameraAngle.y);
	ImGui::SliderFloat("カメラ移動速度X", &cameraSpeed.x, 0.0f, 0.5f);
	ImGui::SliderFloat("カメラ移動速度Y", &cameraSpeed.y, 0.0f, 0.5f);
	ImGui::SliderFloat("ミニマップカメラ距離:%f", &mmCameraDist, -1000.f, 0.f);

	static float saveDist = cameraDist;
	static Vector2 saveAngle = cameraAngle;

	if (ImGui::Checkbox("上から視点に切り替え", &changeCamera) ||
		RInput::GetPadButtonDown(XINPUT_GAMEPAD_BACK))
	{
		//パッド入力の場合はフラグ切り替え1
		if (RInput::GetPadButtonDown(XINPUT_GAMEPAD_BACK)) {
			changeCamera = !changeCamera;
		}

		if (changeCamera)
		{
			saveDist = cameraDist;
			saveAngle = cameraAngle;
		}
		else {
			cameraDist = saveDist;
			cameraAngle = saveAngle;
		}
	}

	if (changeCamera) {
		cameraDist = -160.f;
		cameraAngle.x = 70.f;
		cameraAngle.y = 0.f;
	}

	static Vector3 fpsPos;
	static float fpsDist = cameraDist;
	//static Vector2 fpsAngle = cameraAngle;

	if (ImGui::Checkbox("FPS視点に切り替え", &changeFPS)) {
		if (changeFPS) {
			fpsDist = cameraDist;
			//fpsAngle = cameraAngle;
			fpsPos = camera.mViewProjection.mEye;
		}
		else {
			cameraDist = fpsDist;
			//cameraAngle = fpsAngle;
			camera.mViewProjection.mEye = fpsPos;
		}
	}
	if (changeFPS) {
		camera.mViewProjection.mEye = target->mTransform.position;
		camera.mViewProjection.UpdateMatrix();
		cameraDist = 0.01f;

		if (stick.LengthSq() > 0.0f) {
			float moveSpeed = cameraSpeed.x;

			if (!std::signbit(stick.y)) {
				moveSpeed *= -1;
			}

			cameraAngle.x -= moveSpeed;
		}
	}

	if (ImGui::Button("セーブ")) {
		Parameter::Begin("Camera");
		Parameter::Save("カメラ距離", cameraDist);
		Parameter::Save("プレイヤーからのYのオフセット", cameraUpOffset);
		Parameter::Save("カメラアングルX", cameraAngle.x);
		Parameter::Save("カメラアングルY", cameraAngle.y);
		Parameter::Save("カメラの移動速度X", cameraSpeed.x);
		Parameter::Save("カメラの移動速度Y", cameraSpeed.y);
		Parameter::Save("ミニマップ用カメラ距離", mmCameraDist);
		Parameter::End();
	}

	ImGui::End();
}

void GameCamera::SetTarget(ModelObj* target_)
{
	target = target_;
}

GameCamera* GameCamera::GetInstance()
{
	static GameCamera instance;
	return &instance;
}
