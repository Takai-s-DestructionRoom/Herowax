#include "BreakableObj.h"
#include "Camera.h"
#include "RImGui.h"
#include "Parameter.h"
#include "TimeManager.h"
#include "string"

BreakableObj::BreakableObj()
{
	root = ModelObj(Model::Load("./Resources/Model/Birdnest_Breakable/Birdnest_Breakable.obj", "Birdnest_Breakable"));
	root.mTransform.position = { 0,10,0 };

	std::map<std::string, std::string> extract = Parameter::Extract("breakObj");
	shotPower = Parameter::GetParam(extract,"横の強さ", shotPower);
	gravity = 	Parameter::GetParam(extract, "重力の強さ", gravity);
	minY = Parameter::GetParam(extract, "飛び上がる強さ_min", minY);
	maxY = Parameter::GetParam(extract, "飛び上がる強さ_max", maxY);
	rotSpeed = Parameter::GetParam(extract,"回転速度", rotSpeed);
}

void BreakableObj::Init()
{
	models.clear();
	parts.clear();
	//分割されたデータをそれぞれ子オブジェクトとして管理する
	for (auto& data	 : root.mModel->mData)
	{
		models.emplace_back();
		models.back().mData.push_back(data);
		models.back().mName = root.mModel->mName;
		models.back().mPath = root.mModel->mPath;

		parts.emplace_back();
		parts.back().obj.mModel = &models.back();
		parts.back().obj.mTransform.scale = {5,5,5};
		parts.back().obj.SetParent(&root);
	}
}

void BreakableObj::Update()
{
	//-----------imgui------------//

	ImGui::SetNextWindowSize({ 300, 200 }, ImGuiCond_FirstUseEver);

	ImGui::Begin("BreakableObj");
	
	//破壊
	if (ImGui::Button("破壊")) {
		for (auto& part : parts)
		{
			part.CreateShotVec();
		}
	}
	if (ImGui::Button("リセット")) {
		for (auto& part : parts)
		{
			part.obj.mTransform.position = root.mTransform.position;
			part.obj.mTransform.rotation = root.mTransform.rotation;
		}
	}

	//親を動かす
	ImGui::DragFloat3("親の位置", &root.mTransform.position.x);
	ImGui::DragFloat3("親の大きさ", &root.mTransform.scale.x);
	ImGui::DragFloat3("親の回転", &root.mTransform.rotation.x);

	ImGui::InputFloat("横の強さ", &shotPower,1.0f);
	ImGui::SliderFloat("重力の強さ", &gravity,-10.f,0.0f);
	ImGui::InputFloat("飛び上がる強さ:min", &minY,1.0f);
	ImGui::InputFloat("飛び上がる強さ:max", &maxY,1.0f);
	ImGui::InputFloat("回転速度", &rotSpeed,1.0f);

	if (ImGui::Button("セーブ")) {
		Parameter::Begin("breakObj");
		Parameter::Save("横の強さ", shotPower);
		Parameter::Save("重力の強さ", gravity);
		Parameter::Save("飛び上がる強さ_min", minY);
		Parameter::Save("飛び上がる強さ_max", maxY);
		Parameter::Save("回転速度", rotSpeed);
		Parameter::End();
	}

	for (auto& part : parts)
	{
		part.gravity = gravity;
		part.shotPower = shotPower;
		part.minY = minY;
		part.maxY = maxY;
		part.setRotSpeed = rotSpeed;
	}

	//for (auto& part : parts)
	//{
	//	ImGui::Text("----------%d番-----------", i);
	//	ImGui::DragFloat3(std::string("位置" + std::to_string(i)).c_str(),
	//		&part.obj.mTransform.position.x);
	//	ImGui::DragFloat3(std::string("大きさ" + std::to_string(i)).c_str(),
	//		&part.obj.mTransform.scale.x);
	//	ImGui::DragFloat3(std::string("回転" + std::to_string(i)).c_str(),
	//		&part.obj.mTransform.rotation.x);
	//	i++;
	//}
	
	ImGui::End();

	//-----------imgui------------//

	//更新
	root.mTransform.UpdateMatrix();
	root.TransferBuffer(Camera::sNowCamera->mViewProjection);

	for (auto& part : parts)
	{
		part.Update();
	}
}

void BreakableObj::Draw()
{
	for (auto& part : parts)
	{
		part.Draw();
	}
}

void BreakableObj::SetGround(const ModelObj& ground)
{
	for (auto& part : parts)
	{
		part.SetGroundY(ground.mTransform.position.y);
	}
}

void BreakablePart::Init()
{
}

void BreakablePart::Update()
{
	lowerTimer.Update();
	//moveVec = { 0,0,0 };
	
	moveVec.y += gravity;

	//移動加算
	obj.mTransform.position += moveVec;

	//地面を下回ったら戻す
	if (obj.mTransform.position.y  <= 
		groundY) 
	{
		obj.mTransform.position.y = groundY;
		gravity = 0.0f;
		moveVec = { 0,0,0 };
		shotVec = { 0,0,0 };
		rotSpeed = 0.0f;
	}

	//回転加算
	//指定した軸に指定した強さで回転する
	obj.mTransform.rotation +=
		(rotQuater.ToEuler() * rotSpeed);

	//更新
	obj.mTransform.UpdateMatrix();
	obj.TransferBuffer(Camera::sNowCamera->mViewProjection);
}

void BreakablePart::Draw()
{
	obj.Draw();
}

void BreakablePart::CreateShotVec()
{
	//一旦適当な方向に飛ばす
	Vector3 randVec = { 1,1,1 };
	randVec = Util::GetRandVector3(randVec, -1.f, 1.f);
	shotVec = randVec;
	//Yは上向きにランダム
	shotVec.Normalize();
	shotVec.y = Util::GetRand(minY, maxY);

	moveVec = shotVec * shotPower;

	rotRand = { 1,1,1 };
	rotRand = Util::GetRandVector3(rotRand, -1.f, 1.f);
	rotRand.Normalize();

	rotAngle = Util::GetRand(0.f, Util::PI2);

	rotQuater = Quaternion::AngleAxis(rotRand, rotAngle);

	rotSpeed = setRotSpeed;

	obj.mTransform.rotation = {0.0f,0.0f,0.0f};
	
	lowerTimer.Start();
}
