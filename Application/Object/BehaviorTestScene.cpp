#include "BehaviorTestScene.h"
#include "RImGui.h"
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

//ロードと途中経過の変更
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

	if (RImGui::showImGui)
	{
		ImGui::SetNextWindowSize({ 400, 200 }, ImGuiCond_FirstUseEver);

		ImGui::Begin("敵挙動作成GUI");

		comboFileNames = EnemyBehaviorEditor::LoadFileNames();

		//ImGui::InputText("読み込むパーティクル名", &loadPartName);
		if (!comboFileNames.empty())
		{
			//ハンドルの一覧をプルダウンで表示
			std::vector<const char*> temp;
			for (int32_t i = 0; i < comboFileNames.size(); i++)
			{
				temp.push_back(comboFileNames[i].c_str());
			}
			static int32_t select = 0;
			ImGui::Combo("読み込むオーダー名", &select, &temp[0], (int32_t)comboFileNames.size());
			loadFileName = comboFileNames[select];
		}

		if (ImGui::Button("ロード")) {
			BehaviorData temp = EnemyBehaviorEditor::Load(loadFileName);
			fileName = loadFileName;

			objs.clear();
			lineCube.clear();

			//最後は入れない
			for (int32_t i = 0; i < temp.points.size(); i++)
			{
				CubeCreate(temp.points[i]);
			}
			CubeCreate(temp.points[0]);
		}

		if (ImGui::Button("初期化")) {
			objs.clear();
			lineCube.clear();
			CubeCreate({ 0,0,0 });
			CubeCreate({ 0,0,0 });
		}

		ImGui::DragFloat3("配置オブジェクト:pos", &objs.back().mTransform.position.x, 1.0f);
		if (ImGui::Button("配置")) {
			CubeCreate(objs.back().mTransform.position);
		}

		ImGui::InputText("セーブするファイル名", &fileName);

		if (ImGui::Button("セーブ")) {
			
			int32_t i = 0;
			//一度データを消して、改めて入れ直す
			data.points.clear();
			for (auto& obj : objs)
			{
				//最後は入れない
				if (i == objs.size() - 1)break;
				data.points.push_back(obj.mTransform.position);
				i++;
			}

			EnemyBehaviorEditor::Save(data, fileName);
		}

		//データ一覧表示
		ImGui::Text("セーブ予定のデータ");

		int32_t i = 0;
		for (auto itr = objs.begin(); itr != objs.end() - 1;)
		{
			i++;
			ImGui::Text("--------%d番--------", i);
			std::string str = "座標" + std::to_string(i);
			ImGui::DragFloat3(str.c_str(), &(*itr).mTransform.position.x);
			std::string deleteStr = "このデータを削除" + std::to_string(i);
			if (ImGui::Button(deleteStr.c_str()))
			{
				itr = objs.erase(itr);

				//2点をつなぐキューブを再生成
				LineCubeReCreate();
			}
			else {
				itr++;
			}
		}

		ImGui::End();
	}
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
	//2つ以上オブジェクトが出来たら
	if (objs.size() >= 1) {
		objs.back().mTuneMaterial.mColor = { 1,1,1,1 };

		//表示用にデータを入れる
		data.points.push_back(objs.back().mTransform.position);
	}

	objs.emplace_back();
	objs.back() = ModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube"));

	objs.back().mTransform.position = pos;
	objs.back().mTransform.scale = {1,1,1};
	objs.back().mTuneMaterial.mColor = {1,0,0,1};

	LineCubeReCreate();
}

void BehaviorTestScene::LineCubeReCreate()
{
	//2点をつなぐキューブを描画
	lineCube.clear();
	for (int32_t j = 0; j < objs.size() - 1; j++)
	{
		//2つ目以降なら
		if (j >= 1) {
			lineCube.emplace_back();

			//オブジェクトを作る前に一番後ろのオブジェクトを開始位置に
			lineCube.back().start = &objs[j - 1];

			//オブジェクトを作った後に一番後ろのオブジェクトを終点位置に
			lineCube.back().end = &objs[j];
		}
	}

	returnCube.end = &objs.front();
	returnCube.start = &objs.back() - 1;
	returnCube.SetColor(Color::kLightblue);
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
