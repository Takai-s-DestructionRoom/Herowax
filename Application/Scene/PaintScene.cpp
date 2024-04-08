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

PaintScene::PaintScene()
{
	skydome = PaintableModelObj(Model::Load("./Resources/Model/Skydome/Skydome.obj", "Skydome", true));
	
	sprite = Sprite(TextureManager::Load("./Resources/Brush.png", "brush"), { 0.5f, 0.5f });
	
	objA = PaintableModelObj(Model::Load("./Resources/Model/firewisp/firewisp.obj", "hoge", true));
	objA.mTuneMaterial.mDiffuse = { 0, 1, 0 };
	objA.mTransform.position = { -3, 1, 2 };
	objA.mTransform.UpdateMatrix();
	objB = PaintableModelObj(Model::Load("./Resources/Model/sphere.obj", "Sphere", true));
	objB.mTuneMaterial.mDiffuse = { 1, 0, 1 };
	objB.mTransform.position = { 4, 2, 3 };
	objB.mTransform.scale = { 3.0f, 3.0f, 3.0f };
	objB.mTransform.UpdateMatrix();

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
}

void PaintScene::Update()
{
	ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
	ImGui::SetNextWindowSize({ 400, 250 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;
	ImGui::Begin("Paint Control", NULL, window_flags);
	ImGui::DragFloat("DissolveValue", &dissolveVal, 0.01f, 0.0f, 1.0f);
	ImGui::ColorEdit4("PaintColor", &paintColor.r);
	ImGui::DragFloat3("PaintAmbient", &mAmbient.x, 0.01f);
	ImGui::DragFloat3("PaintDiffuse", &mDiffuse.x, 0.01f);
	ImGui::DragFloat3("PaintSpecular", &mSpecular.x, 0.01f);
	ImGui::End();

	light.Update();
	camera.Update();

	timer += TimeManager::deltaTime;
	objA.mPaintDissolveMapTex = TextureManager::Load("./Resources/DissolveMap.png");
	objA.mPaintDataBuff->slide = timer;
	objA.mPaintDataBuff->dissolveVal = dissolveVal;
	objA.mPaintDataBuff->ambient = mAmbient;
	objA.mPaintDataBuff->diffuse = mDiffuse;
	objA.mPaintDataBuff->specular = mSpecular;
	objA.mPaintDataBuff->color = paintColor;

	skydome.TransferBuffer(camera.mViewProjection);
}

void PaintScene::Draw()
{
	skydome.Draw();

	objA.TransferBuffer(camera.mViewProjection);
	objA.Draw();
	objB.TransferBuffer(camera.mViewProjection);
	objB.Draw();
}
