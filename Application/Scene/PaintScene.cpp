#include "PaintScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include <RenderTarget.h>
#include <Renderer.h>
#include <RImGui.h>

PaintScene::PaintScene()
{
	skydome = ModelObj(Model::Load("./Resources/Model/Skydome/Skydome.obj", "Skydome", true));

	hogeObj = Cube(TextureManager::Load("./Resources/conflict.jpg", "conflict"), {3, 3});
	sprite = Sprite(TextureManager::Load("./Resources/Brush.png", "brush"), { 0.5f, 0.5f });

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

	for (int32_t i = 0; i < 6; i++) {
		Texture tex = TextureManager::Get(hogeObj.mFaces[i].GetTexture());

		UINT width = static_cast<UINT>(tex.mResource.Get()->GetDesc().Width);
		UINT height = tex.mResource.Get()->GetDesc().Height;
		RenderTexture* rt = RenderTarget::CreateRenderTexture(width, height, Color(0, 0, 0, 1));
		tex.Copy(&rt->GetTexture(), RRect(0, static_cast<long>(width), 0, static_cast<long>(height)));
		hogeObj.mFaces[i].SetTexture(rt->mTexHandle);
	}
}

void PaintScene::Update()
{
	light.Update();
	camera.Update();

	skydome.TransferBuffer(camera.mViewProjection);

	hogeObj.UpdateFaces();
	hogeObj.TransferBuffer(camera.mViewProjection);

	if (RInput::GetKey(DIK_P)) {
		int hoge = Util::GetRand(0, 5);
		float width = static_cast<float>(TextureManager::Get(hogeObj.mFaces[hoge].GetTexture()).mResource.Get()->GetDesc().Width);
		float height = static_cast<float>(TextureManager::Get(hogeObj.mFaces[hoge].GetTexture()).mResource.Get()->GetDesc().Height);

		sprite.mTransform.position = { Util::GetRand(0.0f, 1000.0f), Util::GetRand(0.0f, 1000.0f), 0};
		sprite.mTransform.scale = { 0.3f, 0.3f, 1.0f };
		sprite.mTransform.UpdateMatrix();
		sprite.mMaterial.mColor = { 0, 0, 1, 1 };
		sprite.TransferBuffer(Matrix4::OrthoGraphicProjection(0, width, 0, height, 0.0f, 1.0f));

		for (RenderOrder& order : sprite.GetRenderOrder()) {
			order.renderTargets = { hogeObj.mFaces[hoge].GetTexture().substr(16) };
			order.viewports.push_back(Viewport(width, height, 0, 0, 0.0f, 1.0f));
			order.scissorRects.push_back(RRect(0, static_cast<long>(width), 0, static_cast<long>(height)));
			Renderer::DrawCall("Sprite", order);
		}
	}
}

void PaintScene::Draw()
{
	skydome.Draw();

	PipelineStateDesc desc = RDirectX::GetDefPipeline().mDesc;
	desc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	GraphicsPipeline pipe = GraphicsPipeline::GetOrCreate("CubeCull", desc);
	
	for (RenderOrder& order : hogeObj.GetRenderOrder()) {
		order.pipelineState = pipe.mPtr.Get();
		Renderer::DrawCall("Opaque", order);
	}
}
