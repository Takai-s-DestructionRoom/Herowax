#include "RayMarchTestScene.h"
#include "RInput.h"
#include "RImGui.h"
#include "InstantDrawer.h"
#include "TimeManager.h"
#include "Camera.h"
#include "Renderer.h"
#include "Quaternion.h"
#include "Parameter.h"

void RayMarchTestScene::Init()
{
	InstantDrawer::PreCreate();
	
	camera.mViewProjection.mEye = { 0, 0, -10 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();

	skydome = ModelObj(Model::Load("./Resources/Model/Skydome/Skydome.obj", "Skydome"));
	skydome.mTransform.scale = { 5, 5, 5 };
	skydome.mTransform.UpdateMatrix();

	plane = ModelObj(Model::Load("./Resources/Model/Ground/ground.obj", "Ground"));
	plane.mTransform.position = { 0,0.f,0 };
	plane.mTransform.scale = { 10,10,10 };

	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;

	slimeWax.Init();
	
	wasdKeyObj = ModelObj(Model::Load("./Resources/Model/firewisp/firewisp.obj", "firewisp", true));
	wasdKeyObj.mTransform.position = { -10,0,0 };
	
	arrowKeyObj = ModelObj(Model::Load("./Resources/Model/bombSolider/bombSolider.obj", "bombSolider", true));
	arrowKeyObj.mTransform.position = { 10,0,0 };
}

void RayMarchTestScene::Update()
{
	InstantDrawer::DrawInit();

	camera.Update();
	light.Update();

	Vector2 wasdVec = {};
	wasdVec.x = (float)(RInput::GetInstance()->GetKey(DIK_H) - RInput::GetInstance()->GetKey(DIK_F));
	wasdVec.y = (float)(RInput::GetInstance()->GetKey(DIK_T) - RInput::GetInstance()->GetKey(DIK_G));
	//キー入力されてたら
	if (wasdVec.LengthSq() > 0.f) {
		wasdKeyObj.mTransform.position += {wasdVec.x,0, wasdVec.y};
	}
	wasdKeyObj.mTransform.position.y = plane.mTransform.position.y + plane.mTransform.scale.y / 2;

	Vector2 arrowVec = {};
	arrowVec.x = (float)(RInput::GetInstance()->GetKey(DIK_RIGHT) - RInput::GetInstance()->GetKey(DIK_LEFT));
	arrowVec.y = (float)(RInput::GetInstance()->GetKey(DIK_UP) - RInput::GetInstance()->GetKey(DIK_DOWN));
	//キー入力されてたら
	if (arrowVec.LengthSq() > 0.f) {
		arrowKeyObj.mTransform.position += {arrowVec.x, 0, arrowVec.y};
	}
	arrowKeyObj.mTransform.position.y = plane.mTransform.position.y + plane.mTransform.scale.y / 2;

	static float magY = 0.3f;
	spline.clear();
	spline.push_back(wasdKeyObj.mTransform.position);
	//いったん適当な高さでやる 後で距離に応じて高くなるようにする
	Vector3 throwVec = arrowKeyObj.mTransform.position - wasdKeyObj.mTransform.position;
	float range = throwVec.Length();
	throwVec.Normalize();
	
	Vector3 middle = wasdKeyObj.mTransform.position + throwVec * (range / 2);
	middle.y += range * magY;

	spline.push_back(middle);
	spline.push_back(arrowKeyObj.mTransform.position);

	oldTime = timer.GetTimeRate();
	timer.Update();
	if (timer.GetEnd()) {
		oldTime = timer.GetTimeRate();
	}
	if (RInput::GetKeyDown(DIK_N)) {
		timer.Start();
		slimeWax.sphereCenter = wasdKeyObj.mTransform.position;
		for (auto& slime : slimeWax.spheres)
		{
			slime.collider.pos = wasdKeyObj.mTransform.position;
		}
	}

	wasdKeyObj.mTransform.UpdateMatrix();
	wasdKeyObj.TransferBuffer(camera.mViewProjection);

	arrowKeyObj.mTransform.UpdateMatrix();
	arrowKeyObj.TransferBuffer(camera.mViewProjection);

	Vector3 nowSpline = Util::Spline(spline, timer.GetTimeRate());
	Vector3 oldSpline = Util::Spline(spline, oldTime);

	slimeWax.masterMoveVec += nowSpline - oldSpline;

	int32_t i = 0;
	for (auto& slime : slimeWax.spheres)
	{
		//ある程度のやつにディレイを掛ける
		if (slimeWax.spheres.size() / 5 < i && 
			timer.GetTimeRate() < delay1) {
			slime.collider.pos += slimeWax.masterMoveVec;
		}
		if (slimeWax.spheres.size() / 3 < i &&
			timer.GetTimeRate() < delay2) {
			slime.collider.pos += slimeWax.masterMoveVec;
		}
		slime.collider.pos += slimeWax.masterMoveVec;
		i++;
	}

	slimeWax.Update();
	//地面との当たり判定
	for (auto& slime : slimeWax.spheres)
	{
		ColPrimitive3D::Plane planeCol;
		planeCol.normal = {0,1,0};
		planeCol.distance = plane.mTransform.position.y + plane.mTransform.scale.y / 2;
		slime.HitCheck(planeCol);
	}

	plane.mTransform.UpdateMatrix();
	plane.TransferBuffer(camera.mViewProjection);

	skydome.TransferBuffer(camera.mViewProjection);

	ImGui::SetNextWindowSize({ 300, 200 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("plane", NULL);
	ImGui::DragFloat3("板の位置", &plane.mTransform.position.x);
	ImGui::DragFloat3("板の大きさ", &plane.mTransform.scale.x);
	ImGui::DragFloat("magY", &magY,0.1f);
	ImGui::DragFloat("timer", &timer.maxTime_,0.1f);
	ImGui::DragFloat("delay1", &delay1,0.01f);
	ImGui::DragFloat("delay2", &delay2,0.01f);
	ImGui::Checkbox("autoView", &autoView);
	ImGui::End();
}

void RayMarchTestScene::Draw()
{
	skydome.Draw();
	plane.Draw();

	wasdKeyObj.Draw();
	arrowKeyObj.Draw();

	slimeWax.Draw();

	InstantDrawer::AllUpdate();
	InstantDrawer::AllDraw2D();
}

void RayMarchTestScene::Finalize()
{
}
