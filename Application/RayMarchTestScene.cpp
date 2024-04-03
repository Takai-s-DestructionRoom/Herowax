#include "RayMarchTestScene.h"
#include "RInput.h"
#include "RImGui.h"
#include "InstantDrawer.h"
#include "TimeManager.h"
#include "Camera.h"
#include "Renderer.h"
#include "Quaternion.h"

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
}

void RayMarchTestScene::Update()
{
	InstantDrawer::DrawInit();

	camera.Update();
	light.Update();

	slimeWax.Update();
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
	ImGui::End();

}

void RayMarchTestScene::Draw()
{
	skydome.Draw();
	plane.Draw();

	slimeWax.Draw();

	InstantDrawer::AllUpdate();
	InstantDrawer::AllDraw2D();
}

void RayMarchTestScene::Finalize()
{
}
