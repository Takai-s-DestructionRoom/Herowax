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
	
	objA = PaintableModelObj(Model::Load("./Resources/Model/sphere.obj", "Sphere", true));
	objA.mTuneMaterial.mDiffuse = { 0, 1, 0 };
	objA.mTransform.position = { -3, 1, 2 };
	objA.mTransform.UpdateMatrix();
	objB = PaintableModelObj(Model::Load("./Resources/Model/sphere.obj", "Sphere", true));
	objB.mTuneMaterial.mDiffuse = { 1, 0, 1 };
	objB.mTransform.position = { 4, 2, 3 };
	objB.mTransform.scale = { 3.0f, 3.0f, 3.0f };
	objB.mTransform.UpdateMatrix();

	skydome.SetupPaint();
	objA.SetupPaint();
	objB.SetupPaint();

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
	ImGui::SetNextWindowSize({ 400, 150 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;
	ImGui::Begin("Paint Control", NULL, window_flags);
	ImGui::DragFloat("PaintSize", &paintSize);
	ImGui::ColorEdit4("PaintColor", &paintColor.r);
	ImGui::ColorEdit4("ResultColor", &hogeColor.r);
	ImGui::End();

	light.Update();
	camera.Update();

	skydome.TransferBuffer(camera.mViewProjection);

	if (GetActiveWindow() == RWindow::GetWindowHandle()
		&& !ImGui::GetIO().WantCaptureMouse && RInput::GetMouseClick(0)) {
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

		ColPrimitive3D::Ray ray{ mousePosA, (Vector3(mousePosB) - Vector3(mousePosA)).GetNormalize() };

		//skydome.Paint(ray, "brush", paintColor, Vector2(paintSize, paintSize), camera.mViewProjection.mMatrix);
		//objA.Paint(ray, "brush", paintColor, Vector2(paintSize, paintSize), camera.mViewProjection.mMatrix);
		//objB.Paint(ray, "brush", paintColor, Vector2(paintSize, paintSize), camera.mViewProjection.mMatrix);

		int32_t hitMeshIndex = 0;
		int32_t hitIndex = 0;
		float closestDis = FLT_MAX;
		Vector3 closestPos = { 0, 0, 0 };

		for (int32_t mi = 0; mi < skydome.mModel->mData.size(); mi++) {
			ModelMesh& mesh = *skydome.mModel->mData[mi].get();
			std::vector<VertexPNU> verts = mesh.mVertices;
			for (auto& vert : verts) {
				Float4 tp = vert.pos;
				tp *= skydome.mTransform.matrix;
				vert.pos = tp;
				vert.normal *= Matrix4::RotationZXY(skydome.mTransform.rotation.x, skydome.mTransform.rotation.y, skydome.mTransform.rotation.z);
			}

			for (int32_t i = 0; i < mesh.mIndices.size() / 3; i++) {
				VertexPNU vert1 = verts[3 * i];
				VertexPNU vert2 = verts[3 * i + 1];
				VertexPNU vert3 = verts[3 * i + 2];

				Vector3 p0 = vert1.pos;
				Vector3 p1 = vert2.pos;
				Vector3 p2 = vert3.pos;

				Vector3 v1 = p1 - p0;
				Vector3 v2 = p2 - p0;

				Vector3 normal = v1.Cross(v2);
				normal.Normalize();

				ColPrimitive3D::Triangle tri{ vert1.pos, vert2.pos, vert3.pos, normal };

				float outDis;
				Vector3 outInter;
				if (ColPrimitive3D::CheckRayToTriangle(ray, tri, &outDis, &outInter)) {
					if (outDis < closestDis) {
						closestDis = outDis;
						closestPos = outInter;
						hitMeshIndex = mi;
						hitIndex = i;
					}
					break;
				}
			}
		}

		if (closestDis != FLT_MAX) {
			skydome.Paint(closestPos, hitMeshIndex, hitIndex, "brush", paintColor, Vector2(paintSize, paintSize), camera.mViewProjection.mMatrix);
		}
	}

	if (RInput::GetKey(DIK_P)) {
		Util::CalcElapsedTimeStart();
		hogeColor = skydome.ReadPaint({0, 0}, 0);
		Util::CalcElapsedTimeEnd("Read");
	}
}

void PaintScene::Draw()
{
	skydome.Draw();

	objA.TransferBuffer(camera.mViewProjection);
	objA.Draw();
	objB.TransferBuffer(camera.mViewProjection);
	objB.Draw();
}
