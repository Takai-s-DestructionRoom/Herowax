#include "BreakableObj.h"
#include "Camera.h"
#include "RImGui.h"

BreakableObj::BreakableObj()
{
	root = ModelObj(Model::Load("./Resources/Model/Birdnest_Breakable/Birdnest_Breakable.obj", "Birdnest_Breakable"));
	root.mTransform.position = { 0,10,0 };
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

	ImGui::SetNextWindowSize({ 300, 200 });

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("BreakableObj", NULL, window_flags);
	int32_t i = 0;

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
	for (auto& part : parts)
	{
		ImGui::Text("----------%d番-----------", i);
		ImGui::DragFloat3(std::string("位置" + std::to_string(i)).c_str(),
			&part.obj.mTransform.position.x);
		ImGui::DragFloat3(std::string("大きさ" + std::to_string(i)).c_str(),
			&part.obj.mTransform.scale.x);
		ImGui::DragFloat3(std::string("回転" + std::to_string(i)).c_str(),
			&part.obj.mTransform.rotation.x);
		i++;
	}
	
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
	moveVec = { 0,0,0 };
	
	//ショットベクトルはだんだん弱くする
	shotPower = Easing::InQuad(setShotPower, 0.0f, lowerTimer.GetTimeRate());

	//lowerTimerの1.2倍のタイマーで動く
	//上から下のシンプル挙動にしてみる
	gravity = Easing::InQuad(0.0f, -setShotPower, lowerTimer.GetTimeRate());

	if (lowerTimer.GetEnd()) {
		shotPower = 0.0f;
		rotRand = { 0,0,0 };
	}

	//中心から離れるように飛び散る
	moveVec += shotVec * shotPower;
	moveVec.y += shotVec.y * gravity;

	//移動加算
	obj.mTransform.position += moveVec;

	//回転加算
	obj.mTransform.rotation += rotRand * lowerTimer.GetTimeRate();

	//地面を下回ったら戻す
	if (obj.mTransform.position.y <= groundY) {
		obj.mTransform.position.y = groundY;
		gravity = 0.0f;
	}

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
	shotVec.y = 1.0f;
	shotVec.Normalize();
	//Yは上向きに
	gravity = 1.f;

	rotRand = { 1,1,1 };
	rotRand = Util::GetRandVector3(rotRand, -1.f, 1.f);
	rotRand.Normalize();

	lowerTimer.Start();
}
