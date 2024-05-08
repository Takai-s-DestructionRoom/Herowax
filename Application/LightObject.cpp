#include "LightObject.h"
#include "Camera.h"
#include "RImGui.h"
#include "Parameter.h"

void SpotLightObject::Init()
{
	obj = ModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube"));
	isActive = true;
}

void SpotLightObject::Update()
{
	//インデックスが上限値を超えてたら処理を行わない
	if (lightPtr->SPOT_LIGHT_NUM < index) {
		return;
	}
	
	lightPtr->SetSpotLightActive(index, isActive);
	lightPtr->SetSpotLightAtten(index, atten);
	lightPtr->SetSpotLightColor(index, obj.mTuneMaterial.mColor);
	lightPtr->SetSpotLightDirection(index, dir);
	lightPtr->SetSpotLightFactorAngle(index,factorAngle );
	lightPtr->SetSpotLightPos(index,obj.mTransform.position);
	

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void SpotLightObject::Draw()
{
	//インデックスが上限値を超えてたら処理を行わない
	if (LightGroup::SPOT_LIGHT_NUM < index) {
		return;
	}
	obj.Draw();
}

void SpotLightObject::SetIndex(int32_t index_)
{
	index = index_;
}

void SpotLightObject::SetPtr(LightGroup* lightPtr_)
{
	lightPtr = lightPtr_;
}

int32_t SpotLightObject::GetIndex()
{
	return index;
}

void SpotLightManager::Imgui()
{
	if (RImGui::showImGui)
	{
		ImGui::SetNextWindowSize({ 400, 200 }, ImGuiCond_FirstUseEver);

		// デバッグモード //
		ImGui::Begin("ライト");

		if (ImGui::Button("ロード"))
		{
			Load();
		}

		ImGui::Checkbox("dirLightActive", &dirActive);
		lightPtr->SetDirectionalLightActive(0, dirActive);

		ImGui::ColorEdit4("dirLightColor", &dirColor.r);
		lightPtr->SetDirectionalLightColor(0, dirColor);

		ImGui::DragFloat3("spawnPos", &spawnPos.x);

		if (ImGui::Button("生成")) {
			Create(spawnPos, lightPtr);
		}

		for (auto& obj : spotLightObjs)
		{
			std::string name = "active" + std::to_string(obj.GetIndex());
			ImGui::Checkbox(name.c_str(), &obj.isActive);
			name = "position" + std::to_string(obj.GetIndex());
			ImGui::DragFloat3(name.c_str(), &obj.obj.mTransform.position.x);
			name = "atten" + std::to_string(obj.GetIndex());
			ImGui::DragFloat3(name.c_str(), &obj.atten.x, 0.1f);
			name = "dir" + std::to_string(obj.GetIndex());
			ImGui::DragFloat3(name.c_str(), &obj.dir.x);
			name = "factorAngle" + std::to_string(obj.GetIndex());
			ImGui::DragFloat2(name.c_str(), &obj.factorAngle.x,0.01f);
			name = "Color" + std::to_string(obj.GetIndex());
			ImGui::DragFloat4(name.c_str(), &obj.obj.mTuneMaterial.mColor.r);
		}

		if (ImGui::Button("セーブ"))
		{
			Save();
		}

		ImGui::End();
	}
}

void SpotLightManager::Create(Vector3 spawnPos_, LightGroup* lightPtr_)
{
	if (LightGroup::SPOT_LIGHT_NUM < createIndex) {
		return;
	}
	spotLightObjs.emplace_back();
	spotLightObjs.back().Init();
	spotLightObjs.back().obj.mTransform.position = spawnPos_;
	spotLightObjs.back().SetPtr(lightPtr_);
	spotLightObjs.back().SetIndex(createIndex);
	
	createIndex++;
}

void SpotLightManager::Load()
{
	std::map<std::string, std::string> extract = Parameter::Extract("DirLightData");

	dirActive = Parameter::GetParam(extract,"dirLightActive", 1);
	dirColor = Parameter::GetColorData(extract, "dirLightColor", {0,0,0,1});

	spotLightObjs.clear();

	extract = Parameter::Extract("LightData");
	
	for (int32_t i = 0;i < LightGroup::SPOT_LIGHT_NUM;i++)
	{
		std::string name = "exist" + std::to_string(i);
		bool checkExist = Parameter::GetParam(extract, name, 0);
		
		if (checkExist) {
			Create({0,0,0},lightPtr);
			name = "active" + std::to_string(i);
			spotLightObjs.back().isActive = Parameter::GetParam(extract, name, 0);
			name = "position" + std::to_string(i);
			spotLightObjs.back().obj.mTransform.position = Parameter::GetVector3Data(extract, name, {0,0,0});
			name = "atten" + std::to_string(i);
			spotLightObjs.back().atten = Parameter::GetVector3Data(extract, name, { 0,0,0 });
			name = "dir" + std::to_string(i);
			spotLightObjs.back().dir = Parameter::GetVector3Data(extract, name, { 0,0,0 });
			name = "factorAngleX" + std::to_string(i);
			spotLightObjs.back().factorAngle.x = Parameter::GetParam(extract, name, 0);
			name = "factorAngleY" + std::to_string(i);
			spotLightObjs.back().factorAngle.y = Parameter::GetParam(extract, name, 0);
			name = "Color" + std::to_string(i);
			spotLightObjs.back().obj.mTuneMaterial.mColor = Parameter::GetColorData(extract, name, 0);
		}
	}
}

void SpotLightManager::Save()
{
	Parameter::Begin("DirLightData");

	Parameter::Save("dirLightActive", dirActive);
	Parameter::SaveColor("dirLightColor", dirColor);
	
	Parameter::End();

	Parameter::Begin("LightData");
	for (auto& obj : spotLightObjs)
	{
		std::string name = "exist" + std::to_string(obj.GetIndex());
		Parameter::Save(name, 1);
		name = "active" + std::to_string(obj.GetIndex());
		Parameter::Save(name, obj.isActive);
		name = "position" + std::to_string(obj.GetIndex());
		Parameter::SaveVector3(name, obj.obj.mTransform.position);
		name = "atten" + std::to_string(obj.GetIndex());
		Parameter::SaveVector3(name, obj.atten);
		name = "dir" + std::to_string(obj.GetIndex());
		Parameter::SaveVector3(name, obj.dir);
		name = "factorAngleX" + std::to_string(obj.GetIndex());
		Parameter::Save(name,obj.factorAngle.x);
		name = "factorAngleY" + std::to_string(obj.GetIndex());
		Parameter::Save(name, obj.factorAngle.y);
		name = "Color" + std::to_string(obj.GetIndex());
		Parameter::SaveColor(name, obj.obj.mTuneMaterial.mColor);
	}

	Parameter::End();
}

void SpotLightManager::Init(LightGroup* lightPtr_)
{
	SetLightPtr(lightPtr_);
	Load();
}

void SpotLightManager::Update()
{
	for (auto& obj : spotLightObjs)
	{
		obj.Update();
	}
}

void SpotLightManager::Draw()
{
	//imguiでいじってる時だけライトの位置オブジェクトを描画
	if (!RImGui::showImGui)
	{
		return;
	}

	for (auto& obj : spotLightObjs)
	{
		obj.Draw();
	}
}

void SpotLightManager::SetLightPtr(LightGroup* lightPtr_)
{
	lightPtr = lightPtr_;
}
