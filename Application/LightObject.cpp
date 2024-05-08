#include "LightObject.h"
#include "Camera.h"
#include "RImGui.h"
#include "Parameter.h"
#include "BlinkSpotLightObject.h"

void BaseSpotLight::Draw()
{
	//インデックスが上限値を超えてたら処理を行わない
	assert(LightGroup::SPOT_LIGHT_NUM > index);
	if (LightGroup::SPOT_LIGHT_NUM <= index) {
		return;
	}
	obj.Draw();
}

void BaseSpotLight::SetIndex(int32_t index_)
{
	index = index_;
}

void BaseSpotLight::SetPtr(LightGroup* lightPtr_)
{
	lightPtr = lightPtr_;
}

int32_t BaseSpotLight::GetIndex()
{
	return index;
}

void SpotLightObject::Init()
{
	obj = ModelObj(Model::Load("./Resources/Model/Cube.obj", "Cube"));
	isActive = true;
}

void SpotLightObject::Update()
{
	//インデックスが上限値を超えてたら処理を行わない
	assert(LightGroup::SPOT_LIGHT_NUM > index);
	if (lightPtr->SPOT_LIGHT_NUM <= index) {
		return;
	}

	//ライトの値をいじる
	lightPtr->SetSpotLightActive(index, isActive);
	lightPtr->SetSpotLightAtten(index, atten);
	lightPtr->SetSpotLightColor(index, obj.mTuneMaterial.mColor);
	lightPtr->SetSpotLightDirection(index, dir);
	lightPtr->SetSpotLightFactorAngle(index, factorAngle);
	lightPtr->SetSpotLightPos(index, obj.mTransform.position);

	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
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
		ImGui::ColorEdit4("dirLightColor", &dirColor.r);
		
		ImGui::DragFloat3("spawnPos", &spawnPos.x);

		//ハンドルの一覧をプルダウンで表示
		ImGui::Combo("ライトの種類", &comboSelect, &typeCombo[0], (int32_t)typeCombo.size());
		lightType = typeCombo[comboSelect];
		
		if (ImGui::Button("生成")) {
			Create(spawnPos, lightType,lightPtr);
			lightType = "";
		}

		for (auto& obj : spotLightObjs)
		{
			std::string name = "active" + std::to_string(obj->GetIndex());
			ImGui::Checkbox(name.c_str(), &obj->isActive);
			name = "position" + std::to_string(obj->GetIndex());
			ImGui::DragFloat3(name.c_str(), &obj->obj.mTransform.position.x);
			name = "atten" + std::to_string(obj->GetIndex());
			ImGui::DragFloat3(name.c_str(), &obj->atten.x, 0.1f);
			name = "dir" + std::to_string(obj->GetIndex());
			ImGui::DragFloat3(name.c_str(), &obj->dir.x);
			name = "factorAngle" + std::to_string(obj->GetIndex());
			ImGui::DragFloat2(name.c_str(), &obj->factorAngle.x,0.01f);
			name = "Color" + std::to_string(obj->GetIndex());
			ImGui::DragFloat4(name.c_str(), &obj->obj.mTuneMaterial.mColor.r);
		}

		if (ImGui::Button("セーブ"))
		{
			Save();
		}

		ImGui::End();
	}
}

void SpotLightManager::Create(Vector3 spawnPos_, std::string lightType_, LightGroup* lightPtr_)
{
	assert(LightGroup::SPOT_LIGHT_NUM > createIndex);
	if (LightGroup::SPOT_LIGHT_NUM <= createIndex) {
		return;
	}
	spotLightObjs.emplace_back();
	if (lightType_ == "Blink") {
		spotLightObjs.back() = std::make_unique<BlinkSpotLightObject>();
	}
	if (lightType_ == "Normal") {
		spotLightObjs.back() = std::make_unique<SpotLightObject>();
	}
	if (lightType_ == "") {
		spotLightObjs.back() = std::make_unique<SpotLightObject>();
	}
	spotLightObjs.back()->Init();
	spotLightObjs.back()->obj.mTransform.position = spawnPos_;
	spotLightObjs.back()->SetPtr(lightPtr_);
	spotLightObjs.back()->SetIndex(createIndex);
	
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
			name = "lightType" + std::to_string(i);
			Create({0,0,0}, Parameter::GetParamStr(extract, name, ""), lightPtr);
			name = "active" + std::to_string(i);
			spotLightObjs.back()->isActive = Parameter::GetParam(extract, name, 0);
			name = "position" + std::to_string(i);
			spotLightObjs.back()->obj.mTransform.position = Parameter::GetVector3Data(extract, name, {0,0,0});
			name = "atten" + std::to_string(i);
			spotLightObjs.back()->atten = Parameter::GetVector3Data(extract, name, { 0,0,0 });
			name = "dir" + std::to_string(i);
			spotLightObjs.back()->dir = Parameter::GetVector3Data(extract, name, { 0,0,0 });
			name = "factorAngleX" + std::to_string(i);
			spotLightObjs.back()->factorAngle.x = Parameter::GetParam(extract, name, 0);
			name = "factorAngleY" + std::to_string(i);
			spotLightObjs.back()->factorAngle.y = Parameter::GetParam(extract, name, 0);
			name = "Color" + std::to_string(i);
			spotLightObjs.back()->obj.mTuneMaterial.mColor = Parameter::GetColorData(extract, name, 0);
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
		std::string name = "exist" + std::to_string(obj->GetIndex());
		Parameter::Save(name, 1);
		name = "active" + std::to_string(obj->GetIndex());
		Parameter::Save(name, obj->isActive);
		name = "position" + std::to_string(obj->GetIndex());
		Parameter::SaveVector3(name, obj->obj.mTransform.position);
		name = "atten" + std::to_string(obj->GetIndex());
		Parameter::SaveVector3(name, obj->atten);
		name = "dir" + std::to_string(obj->GetIndex());
		Parameter::SaveVector3(name, obj->dir);
		name = "factorAngleX" + std::to_string(obj->GetIndex());
		Parameter::Save(name,obj->factorAngle.x);
		name = "factorAngleY" + std::to_string(obj->GetIndex());
		Parameter::Save(name, obj->factorAngle.y);
		name = "Color" + std::to_string(obj->GetIndex());
		Parameter::SaveColor(name, obj->obj.mTuneMaterial.mColor);
	}

	Parameter::End();
}

SpotLightObject* SpotLightManager::GetLight(int32_t index)
{
	for (auto& obj : spotLightObjs)
	{
		if (obj->GetIndex() == index) {
			return obj.get();
		}
	}
	return nullptr;
}

SpotLightManager* SpotLightManager::GetInstance()
{
	static SpotLightManager instance;
	return &instance;
}

void SpotLightManager::Init(LightGroup* lightPtr_)
{
	SetLightPtr(lightPtr_);
	Load();

	comboSelect = 0;
	createIndex = 0;
}

void SpotLightManager::Update()
{
	lightPtr->SetDirectionalLightActive(0, dirActive);
	lightPtr->SetDirectionalLightColor(0, dirColor);

	for (auto& obj : spotLightObjs)
	{
		obj->Update();
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
		obj->Draw();
	}
}

void SpotLightManager::SetLightPtr(LightGroup* lightPtr_)
{
	lightPtr = lightPtr_;
}