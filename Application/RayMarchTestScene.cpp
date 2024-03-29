#include "RayMarchTestScene.h"
#include "RInput.h"
#include "RImGui.h"
#include "InstantDrawer.h"
#include "TimeManager.h"

void RayMarchTestScene::Init()
{
	PhysicsCircle::LoadResource();
	InstantDrawer::PreCreate();

	//画面外から伸ばすレイ
	ground.start = {Util::WIN_WIDTH * 2,Util::WIN_HEIGHT /2,0};
	ground.dir = {-1,0.f,0.f};
}

void RayMarchTestScene::Update()
{
	InstantDrawer::DrawInit();

	//マウスで球を動かす
	/*if (RInput::GetMouseClickDown(1)) {
		downPos = RInput::GetMousePos();
	}
	if (RInput::GetMouseClickUp(1)) {
		circle.addImpulse(RInput::GetMousePos() - downPos, downPos);
	}*/

	if (RInput::GetMouseClickDown(1)) {
		circles.emplace_back(RInput::GetMousePos(), 3.f);
	}

	for (auto& circle : circles)
	{
		circle.v.y += 9.8f * 60.f * TimeManager::deltaTime;//重力
	}

	for (auto& circle : circles)
	{
		//地面との判定
		float dist = 0.f;
		Vector3 inter = { 0,0,0 };
		if (ColPrimitive3D::CheckRayToSphere(ground, circle.sphere,
			&dist, &inter)) {
			//交点から球の中心へ向けた法線
			Vector2 subV = circle.sphere.pos - inter;
			Vector2 nv = subV.GetNormalize();

			float overlap = circle.r - subV.Length();

			circle.pos += nv * overlap;
			float dotV = -circle.v.Dot(nv);
			//念のため、衝突方向を向いていないなら虫
			if (dotV >= 0) {
				//circle.addImpulseLocal(nv * dotV * circle.M, -nv * circle.r);
				Vector2 tan = nv.Rotation(Util::AngleToRadian(180.f));
				Vector2 fDir = -(float)std::signbit(circle.v.Dot(tan) - circle.r * circle.omega) * tan;
				circle.addImpulseLocal((nv + fDir * 0.5f) * min(dotV, 50.f) * circle.M, -nv * circle.r);//摩擦+垂直効力
		
				//circle.v.y = 0.0f;
			}
		}

		circle.Update();
	}

	ImGui::SetNextWindowSize({ 300, 200 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("circle", NULL, window_flags);
	ImGui::SliderFloat2("rayDir", &ground.dir.x, -1.0f, 1.0f);
	if (ImGui::Button("Reset")) {
		circles.clear();
	}
	ImGui::End();
}

void RayMarchTestScene::Draw()
{
	for (auto& circle : circles)
	{
		circle.Draw();
	}

	for (int32_t i = 0; i < 10; i++)
	{
		InstantDrawer::DrawGraph(
			ground.start.x + (ground.dir.x * i * 10.f),
			ground.start.y + (ground.dir.y * i * 10.f),
			3.f, 3.f, 0, "circle", Color(1, 0, 0, 1));
	}

	InstantDrawer::AllUpdate();
	InstantDrawer::AllDraw2D();
}

void RayMarchTestScene::Finalize()
{
}
