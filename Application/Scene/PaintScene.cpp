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
	skydome = ModelObj(Model::Load("./Resources/Model/Skydome/Skydome.obj", "Skydome", true));

	sphere = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));
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
	ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
	ImGui::SetNextWindowSize({ 400, 150 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;
	ImGui::Begin("Paint Control", NULL, window_flags);
	ImGui::DragFloat3("ObjScale", &hogeObj.mTransform.scale.x, 0.05f);
	ImGui::DragFloat("PaintSize", &paintSize);
	ImGui::ColorEdit3("PaintColor", &paintColor.r);
	ImGui::End();

	light.Update();
	camera.Update();

	skydome.TransferBuffer(camera.mViewProjection);

	hogeObj.mTransform.UpdateMatrix();
	hogeObj.UpdateFaces();
	hogeObj.TransferBuffer(camera.mViewProjection);

	sphere.mTransform.scale = { 0.2f, 0.2f, 0.2f };

	if (RInput::GetMouseClick(0)) {
		Float4 mousePosA = RInput::GetMousePos();
		Float4 mousePosB = RInput::GetMousePos();
		mousePosB.z = 1;

		mousePosA *= -Matrix4::Viewport(0, 0, 1280, 720, 0.0f, 1.0f);
		mousePosA *= -camera.mViewProjection.mProjection;
		mousePosA /= mousePosA.w;
		mousePosA *= -camera.mViewProjection.mView;

		mousePosB *= -Matrix4::Viewport(0, 0, 1280, 720, 0.0f, 1.0f);
		mousePosB *= -camera.mViewProjection.mProjection;
		mousePosB /= mousePosB.w;
		mousePosB *= -camera.mViewProjection.mView;

		ColPrimitive3D::Ray ray{ mousePosA, (Vector3(mousePosB) - Vector3(mousePosA)).GetNormalize()};

		int32_t hitFace = 0;
		float closestDis = FLT_MAX;
		Vector3 closestPos = { 0, 0, 0 };
		Vector2 resultUV;
		for (int32_t i = 0; i < 6; i++) {
			std::vector<VertexPNU> verts = hogeObj.mFaces[i].GetVertices();

			//0, 2, 1
			//1, 2, 3

			for (int32_t j = 0; j < 4; j++) {
				verts[j].pos *= hogeObj.mFaces[i].mTransform.matrix;
				Vector3 rot = hogeObj.mFaces[i].mTransform.rotation;
				verts[j].normal *= Matrix4::RotationZXY(rot.x, rot.y, rot.z);
			}

			ColPrimitive3D::Triangle ta{ verts[0].pos, verts[2].pos, verts[1].pos, verts[1].normal };
			ColPrimitive3D::Triangle tb{ verts[1].pos, verts[2].pos, verts[3].pos, verts[1].normal };

			float outDis;
			Vector3 outInter;
			if (ColPrimitive3D::CheckRayToTriangle(ray, ta, &outDis, &outInter)) {
				if (outDis < closestDis) {
					closestDis = outDis;
					closestPos = outInter;
					hitFace = i;
					resultUV = PaintUtil::CalcPointTexCoord(
						{ verts[0].pos, verts[2].pos, verts[1].pos },
						{ verts[0].uv, verts[2].uv, verts[1].uv },
						outInter, camera.mViewProjection.mMatrix);
				}
				break;
			}
			if (ColPrimitive3D::CheckRayToTriangle(ray, tb, &outDis, &outInter)) {
				if (outDis < closestDis) {
					closestDis = outDis;
					closestPos = outInter;
					hitFace = i;
					resultUV = PaintUtil::CalcPointTexCoord(
						{ verts[1].pos, verts[2].pos, verts[3].pos },
						{ verts[1].uv, verts[2].uv, verts[3].uv },
						outInter, camera.mViewProjection.mMatrix);
				}
				break;
			}
		}

		if (closestDis != FLT_MAX) {
			sphere.mTransform.position = closestPos;

			float width = static_cast<float>(TextureManager::Get(hogeObj.mFaces[hitFace].GetTexture()).mResource.Get()->GetDesc().Width);
			float height = static_cast<float>(TextureManager::Get(hogeObj.mFaces[hitFace].GetTexture()).mResource.Get()->GetDesc().Height);
			sprite.mTransform.position = {
				width * resultUV.x,
				height * resultUV.y,
				0.0f
			};
			sprite.mTransform.scale = { paintSize / 1024.0f / hogeObj.mTransform.scale.x, paintSize / 1024.0f / hogeObj.mTransform.scale.y, 1.0f };
			sprite.mTransform.UpdateMatrix();
			sprite.mMaterial.mColor = paintColor;
			sprite.TransferBuffer(Matrix4::OrthoGraphicProjection(0, width, 0, height, 0.0f, 1.0f));

			for (RenderOrder& order : sprite.GetRenderOrder()) {
				order.renderTargets = { hogeObj.mFaces[hitFace].GetTexture().substr(16) };
				order.viewports.push_back(Viewport(width, height, 0, 0, 0.0f, 1.0f));
				order.scissorRects.push_back(RRect(0, static_cast<long>(width), 0, static_cast<long>(height)));
				Renderer::DrawCall("Sprite", order);
			}
		}
		else {
			sphere.mTransform.position = { 114514, 114514, 0 };
		}
	}

	sphere.mTransform.UpdateMatrix();
	sphere.TransferBuffer(camera.mViewProjection);

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
	//skydome.Draw();

	sphere.Draw();

	PipelineStateDesc desc = RDirectX::GetDefPipeline().mDesc;
	desc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	GraphicsPipeline pipe = GraphicsPipeline::GetOrCreate("CubeCull", desc);
	
	for (RenderOrder& order : hogeObj.GetRenderOrder()) {
		order.pipelineState = pipe.mPtr.Get();
		Renderer::DrawCall("Opaque", order);
	}
}
