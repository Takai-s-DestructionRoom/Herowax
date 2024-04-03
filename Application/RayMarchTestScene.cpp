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
}

void RayMarchTestScene::Draw()
{
	slimeWax.Draw();

	InstantDrawer::AllUpdate();
	InstantDrawer::AllDraw2D();
}

void RayMarchTestScene::Finalize()
{
}
