#include "BehaviorTestScene.h"
//#include "RImGui.h"
#include "Camera.h"
#include "Easing.h"

void BehaviorTestScene::Init()
{
	CubeCreate({ 0,0,0 });
	CubeCreate({ 0,0,0 });

	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;

	skydome = ModelObj(Model::Load("./Resources/Model/Skydome/Skydome.obj", "Skydome"));
	skydome.mTransform.scale = { 5, 5, 5 };
	skydome.mTransform.UpdateMatrix();
}

void BehaviorTestScene::Update()
{
	camera.Update();
	light.Update();

	for (auto& obj : objs)
	{
		obj.mTransform.UpdateMatrix();
		obj.TransferBuffer(camera.mViewProjection);
	}
	for (auto& cube : lineCube)
	{
		cube.Update();
	}
	returnCube.Update();

	skydome.TransferBuffer(camera.mViewProjection);

//#pragma region ImGui
//	ImGui::SetNextWindowSize({ 400, 200 }, ImGuiCond_FirstUseEver);
//
//	ImGui::Begin("敵挙動作成GUI");
//
//	if (ImGui::Button("初期化")) {
//		objs.clear();
//		lineCube.clear();
//		CubeCreate({ 0,0,0 });
//		CubeCreate({ 0,0,0 });
//	}
//
//	ImGui::DragFloat3("配置オブジェクト:pos", &objs.back().mTransform.position.x, 1.0f);
//	if (ImGui::Button("配置")) {
//		CubeCreate(objs.back().mTransform.position);
//	}
//
//	ImGui::InputText("セーブするファイル名", &fileName);
//
//	if (ImGui::Button("セーブ")) {
//		int32_t i = 0;
//		data.points.clear();
//		for (auto& obj : objs)
//		{
//			//最後は入れない
//			if (i == objs.size() - 1)break;
//			data.points.push_back(obj.mTransform.position);
//			i++;
//		}
//
//		EnemyBehaviorEditor::Save(data, fileName);
//	}
//
//	ImGui::End();
//#pragma endregion
}

void BehaviorTestScene::Draw()
{
	skydome.Draw();

	for (auto& obj : objs)
	{
		obj.Draw();
	}
	
	for (auto& cube : lineCube)
	{
		cube.Draw();
	}
	returnCube.Draw();
}

void BehaviorTestScene::Finalize()
{
}

void BehaviorTestScene::CubeCreate(Vector3 pos)
{
	lineCube.emplace_back();
	//2つ以上オブジェクトが出来たら
	if (objs.size() >= 1) {
		//オブジェクトを作る前に一番後ろのオブジェクトを開始位置に
		lineCube.back().start = &objs.back();

		objs.back().mTuneMaterial.mColor = { 1,1,1,1 };

		returnCube.end = &objs.front();
		returnCube.start = &objs.back();
		returnCube.SetColor(Color::kLightblue);
	}

	objs.emplace_back();
	objs.back() = ModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube"));

	objs.back().mTransform.position = pos;
	objs.back().mTransform.scale = {1,1,1};
	objs.back().mTuneMaterial.mColor = {1,0,0,1};

	//オブジェクトを作った後に一番後ろのオブジェクトを終点位置に
	lineCube.back().end = &objs.back();
}

EasingCube::EasingCube()
{
	obj = ModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube"));
	obj.mTransform.scale = { 0.5f,0.5f ,0.5f };
	obj.mTuneMaterial.mColor = { 1,1,1,1.0f };
	timer.Start();
}

void EasingCube::Update()
{
	timer.Roop();

	if (start != nullptr && end != nullptr)
	{
		obj.mTransform.position = InQuadVec3(
			start->mTransform.position,
			end->mTransform.position,
			timer.GetTimeRate());
	}

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void EasingCube::Draw()
{
	obj.Draw("Transparent");
}
