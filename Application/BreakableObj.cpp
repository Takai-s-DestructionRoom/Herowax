#include "BreakableObj.h"
#include "Camera.h"
#include "RImGui.h"

BreakableObj::BreakableObj()
{
	root = *ModelManager::Get(Model::Load("./Resources/Model/Birdnest_Breakable/Birdnest_Breakable.obj", "Birdnest_Breakable"));
}

void BreakableObj::Init()
{
	models.clear();
	objs.clear();
	//分割されたデータをそれぞれ子オブジェクトとして管理する
	for (auto& data	 : root.mData)
	{
		models.emplace_back();
		models.back().mData.push_back(data);
		models.back().mName = root.mName;
		models.back().mPath = root.mPath;

		objs.emplace_back();
		objs.back().mModel = &models.back();
		objs.back().mTransform.position = {0,0,-10};
		objs.back().mTransform.scale = {5,5,5};
		int32_t hoge=0;
		hoge++;
	}
}

void BreakableObj::Update()
{
	ImGui::SetNextWindowSize({ 300, 200 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("BreakableObj", NULL, window_flags);
	int32_t i = 0;

	for (auto& obj : objs)
	{
		ImGui::Text("----------%d番-----------", i);
		std::string name1 = "位置" + std::to_string(i);
		std::string name2 = "大きさ" + std::to_string(i);
		ImGui::DragFloat3(name1.c_str(), &obj.mTransform.position.x);
		ImGui::DragFloat3(name2.c_str(), &obj.mTransform.scale.x);
		ImGui::DragFloat3(std::string("回転" + std::to_string(i)).c_str(),
			&obj.mTransform.rotation.x);
		i++;
	}
	
	ImGui::End();

	for (auto& obj : objs)
	{
		obj.mTransform.UpdateMatrix();
		obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
	}
}

void BreakableObj::Draw()
{
	for (auto& obj : objs)
	{
		obj.Draw();
	}
}
