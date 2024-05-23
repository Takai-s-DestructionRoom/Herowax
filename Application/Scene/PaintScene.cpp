#include "PaintScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include <RenderTarget.h>
#include <Renderer.h>
#include <RImGui.h>
#include <ColPrimitive3D.h>
#include <Float4.h>
#include <PaintUtil.h>
#include "MetaBall2D.h"

PaintScene::PaintScene()
{
	//skydome = PaintableModelObj(Model::Load("./Resources/Model/Skydome/Skydome.obj", "Skydome", true));
	
	camera.mViewProjection.mEye = { 0, 0, -10 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();
}

PaintScene::~PaintScene()
{
}

void PaintScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
	MetaBall2D::GetInstance()->Init();
}

void PaintScene::Update()
{
	camera.Update();
	light.Update();

	/*skydome.mTransform.UpdateMatrix();
	skydome.TransferBuffer(camera.mViewProjection);*/

	MetaBall2D::GetInstance()->Update();
}

void PaintScene::Draw()
{
	//skydome.Draw();
	MetaBall2D::GetInstance()->Draw();
}
