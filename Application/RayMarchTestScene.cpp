#include "RayMarchTestScene.h"
#include "RInput.h"
#include "RImGui.h"
#include "InstantDrawer.h"
#include "TimeManager.h"
#include "Camera.h"
#include "Renderer.h"
#include "Quaternion.h"
#include "Parameter.h"
#include "WaxManager.h"

void RayMarchTestScene::Init()
{
	InstantDrawer::PreCreate();

	camera.mViewProjection.mEye = { 0, 25, -20 };
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

	WaxManager::GetInstance()->slimeWax.Init();

	debugObj.Init();
}

void RayMarchTestScene::Update()
{
	InstantDrawer::DrawInit();
	WaxManager::GetInstance()->slimeWax.Reset();

	camera.Update();
	light.Update();
	debugObj.Update();
	
	//地面との当たり判定
	for (auto& slime : WaxManager::GetInstance()->slimeWax.spheres)
	{
		ColPrimitive3D::Plane planeCol;
		planeCol.normal = {0,1,0};
		planeCol.distance = plane.mTransform.position.y + plane.mTransform.scale.y / 2;
		slime.HitCheck(planeCol);
	}

	createTimer.Update();
	if (!createTimer.GetRun()) {
		createTimer.Start();
		CreateWaxVisual();
	}

	for (auto itr = waxVisual.begin(); itr != waxVisual.end();)
	{
		//死んでたら殺す
		if (!(itr->get())->isAlive) {
			itr = waxVisual.erase(itr);
		}
		else {
			itr++;
		}
	}

	for (auto& wax1 : waxVisual)
	{
		for (auto& wax2 : waxVisual)
		{
			if (wax1 == wax2)continue;

			//重なりチェック
			if (ColPrimitive3D::CheckSphereToSphere(wax1->collider, wax2->collider))
			{
				wax1->power++;
				wax2->power++;
			}
		}
	}

	for (auto& wax : waxVisual)
	{
		bool check = false;
		wax->SetGround(plane.mTransform);
		wax->Update();

		//当たり判定
		while (ColPrimitive3D::CheckSphereToSphere(wax->collider, debugObj.collider))
		{
			Vector3 repulsionVec = wax->collider.pos - debugObj.obj.mTransform.position;
			repulsionVec.Normalize();
			repulsionVec.y = 0;

			wax->collider.pos += repulsionVec;
			//wax.obj.mTransform.position += repulsionVec;

			//もしここが0になった場合無限ループするので抜ける
			if (repulsionVec.LengthSq() == 0) {
				break;
			}
			check = true;
			wax->obj.SetParent(&debugObj.obj);
		}

		wax->TransferBuffer();

		////送った後、このフレームで当たっていないなら親子を解除
		//if (!check) {
		//	wax.obj.SetParent(nullptr);
		//}
	}

	WaxManager::GetInstance()->slimeWax.Update();

	plane.mTransform.UpdateMatrix();
	plane.TransferBuffer(camera.mViewProjection);

	skydome.TransferBuffer(camera.mViewProjection);

	if (RImGui::showImGui)
	{
		ImGui::SetNextWindowSize({ 300, 200 }, ImGuiCond_FirstUseEver);

		ImGui::Begin("plane");
		ImGui::DragFloat3("板の位置", &plane.mTransform.position.x);
		ImGui::DragFloat3("板の大きさ", &plane.mTransform.scale.x);
		ImGui::Checkbox("autoView", &autoView);
		ImGui::DragFloat3("debugObj:位置", &debugObj.obj.mTransform.position.x);
		ImGui::DragFloat("debugObj:コライダーサイズ", &debugObj.colliderSize);
		ImGui::DragFloat("createTimer;maxTime", &createTimer.maxTime_);
		if (ImGui::Button("全消し")) {
			waxVisual.clear();
		}
		ImGui::End();
	}
}

void RayMarchTestScene::Draw()
{
	skydome.Draw();
	plane.Draw();
	debugObj.Draw();
	
	WaxManager::GetInstance()->slimeWax.Draw();

	InstantDrawer::AllUpdate();
	InstantDrawer::AllDraw2D();
}

void RayMarchTestScene::Finalize()
{
}

void RayMarchTestScene::CreateWaxVisual()
{
	waxVisual.emplace_back();
	//差分だけ保持
	waxVisual.back() = std::make_unique<WaxVisual>();
	waxVisual.back()->obj.SetParent(&debugObj.obj);
	waxVisual.back()->Init();
}

void DebugObject::Init()
{
	obj = PaintableModelObj(Model::Load("./Resources/Model/Cube.obj","Cube"));
	isDrawCollider = true;

	obj.mTransform.position = { 0,28,0 };
	colliderSize = 4.f;
}

void DebugObject::Update()
{
	UpdateCollider();
	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void DebugObject::Draw()
{
	obj.Draw();
	DrawCollider();
}
