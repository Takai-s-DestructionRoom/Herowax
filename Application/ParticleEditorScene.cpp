#include "ParticleEditorScene.h"
#include "ParticleEditor.h"
#include "RImGui.h"
#include "ParticleManager.h"
#include "RInput.h"
#include "Renderer.h"

void ParticleEditorScene::Init()
{
	ParticleManager::GetInstance()->Init();
	ParticleManager::GetInstance()->SetPlayerPos({ 0,0,0 });

	camera.mViewProjection.mEye = { 0, 0, -10 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();

	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;

	drawEmitter = ModelObj(Model::Load("./Resources/Model/Cube.obj","Cube"));
	drawEmitter.mTransform.scale = { 1,1,1 };

	ParticleEditor::Init();	//初期化
}

void ParticleEditorScene::Update()
{
	//カメラをこのシーン用に改良
	if (RInput::GetKeyDown(DIK_F3)) {
		camForce = !camForce;
	}
	camera.mFreeFlag = camForce;
	if (camForce) {
		RWindow::SetMouseLock(false);
	}
	camera.Update();
	light.Update();
	
	drawEmitter.mTransform.position = emitPos;
	drawEmitter.mTransform.UpdateMatrix();
	drawEmitter.TransferBuffer(camera.mViewProjection);
	
	ParticleEditor::OrderCreateGUI();
	roopTimer.Update();

	if (isAutoCreate) {
		if (!roopTimer.GetRun()) {
			roopTimer.Start();

			//ファイル名に"_ring"が含まれてるなら円形パーティクルを出す
			if (Util::ContainString(loadPartName, "_ring")) {
				ParticleManager::GetInstance()->AddRing(emitPos, loadPartName);
			}
			//それ以外はシンプルパーティクルとみなす
			else if (loadPartName != "") {
				ParticleManager::GetInstance()->AddSimple(emitPos, loadPartName);
				drawEmitter.mTransform.scale = ParticleEditor::LoadSimple(loadPartName).emitScale * 2.f;
			}
		}
	}

	ParticleManager::GetInstance()->Update();

	ImGui::SetNextWindowSize({ 400, 200 }, ImGuiCond_FirstUseEver);

	ImGui::Begin("パーティクル生成GUI");

	fileNames = ParticleEditor::LoadFileNames();

	//ImGui::InputText("読み込むパーティクル名", &loadPartName);
	if (!fileNames.empty())
	{
		//ハンドルの一覧をプルダウンで表示
		std::vector<const char*> temp;
		for (size_t i = 0; i < fileNames.size(); i++)
		{
			temp.push_back(fileNames[i].c_str());
		}
		static int32_t select = 0;
		ImGui::Combo("読み込むパーティクル名", &select, &temp[0], (int32_t)fileNames.size());
		loadPartName = fileNames[select];
	}
	ImGui::DragFloat3("生成位置", &emitPos.x);
	if (ImGui::Button("一回生成")) {
		//ファイル名に"_ring"が含まれてるなら円形パーティクルを出す
		if (Util::ContainString(loadPartName,"_ring")) {
			ParticleManager::GetInstance()->AddRing(emitPos, loadPartName);
		}
		//それ以外はシンプルパーティクルとみなす
		else if (loadPartName != "") {
			ParticleManager::GetInstance()->AddSimple(emitPos, loadPartName);
			drawEmitter.mTransform.scale = ParticleEditor::LoadSimple(loadPartName).emitScale * 2.f;
		}
	}
	ImGui::Checkbox("無限に生成するか", &isAutoCreate);
	ImGui::InputFloat("無限生成の間隔", &roopTimer.maxTime_,0.1f);
	ImGui::Text("カメラ固定はF3でも切り替えられます");
	ImGui::Checkbox("カメラ固定を解除", &camForce);
	ImGui::End();
}

void ParticleEditorScene::Draw()
{
	//パイプラインをワイヤーフレームに
	PipelineStateDesc pipedesc = RDirectX::GetDefPipeline().mDesc;
	pipedesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

	GraphicsPipeline pipe = GraphicsPipeline::GetOrCreate("WireObject", pipedesc);
	for (RenderOrder& order : drawEmitter.GetRenderOrder()) {
		order.pipelineState = pipe.mPtr.Get();
		Renderer::DrawCall("Opaque", order);
	}
	//drawEmitter.Draw();
	ParticleManager::GetInstance()->Draw();
}

void ParticleEditorScene::Finalize()
{
}
