#include "MainTestScene.h"
#include "RInput.h"
#include "TextDrawer.h"
#include "TimeManager.h"
#include "SimpleDrawer.h"
#include <RenderTarget.h>
#include <Renderer.h>
#include <RImGui.h>

MainTestScene::MainTestScene()
{
	skydome = ModelObj(Model::Load("./Resources/Model/Skydome/Skydome.obj", "Skydome", true));
	testModel = ModelObj(Model::Load("./Resources/Model/firewisp/firewisp.obj", "testModel", true));

	camera.mViewProjection.mEye = { 0, 0, -10 };
	camera.mViewProjection.mTarget = { 0, 0, 0 };
	camera.mViewProjection.UpdateMatrix();
}

MainTestScene::~MainTestScene()
{

}

void MainTestScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;

	meltBuff->factor = 1;
	meltBuff->falloff = 1;
	meltBuff->radius = 1;
	meltBuff->clampAtBottom = true;
	meltBuff->top = 1;
	meltBuff->bottom = 0;
}

void MainTestScene::Update()
{
	ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
	ImGui::SetNextWindowSize({ 400, 500 }, ImGuiCond_FirstUseEver);

	ImGui::Begin("Melt Control");
	ImGui::Text("modelAxis");
	ImGui::DragFloat3("Pos##model", &testModel.mTransform.position.x, 0.05f);
	static Vector3 lRot;
	ImGui::DragFloat3("Rot##model", &lRot.x, 0.5f);
	testModel.mTransform.rotation = { Util::AngleToRadian(lRot.x), Util::AngleToRadian(lRot.y), Util::AngleToRadian(lRot.z) };
	ImGui::DragFloat3("Sca##model", &testModel.mTransform.scale.x, 0.05f);
	ImGui::Checkbox("WireFrame", &useWireframe);
	ImGui::Separator();
	ImGui::Text("meltAxis");
	ImGui::DragFloat3("Pos##melt", &meltAxis.position.x, 0.05f);
	ImGui::DragFloat3("Rot##melt", &meltAxis.rotation.x, 0.5f);
	ImGui::DragFloat3("Sca##melt", &meltAxis.scale.x, 0.05f);
	ImGui::Separator();
	ImGui::Text("meltParam");
	ImGui::DragFloat("factor", &meltBuff->factor, 0.05f, 0, 1);
	ImGui::DragFloat("falloff", &meltBuff->falloff, 0.05f);
	ImGui::DragFloat("radius", &meltBuff->radius, 0.05f);
	ImGui::DragFloat("top", &meltBuff->top, 0.05f);
	ImGui::DragFloat("bottom", &meltBuff->bottom, 0.05f);
	ImGui::Checkbox("clampAtBottom", &meltBuff->clampAtBottom);
	ImGui::End();

	light.Update();
	camera.Update();

	testModel.mTransform.UpdateMatrix();
	meltAxis.UpdateMatrix();
	meltBuff->matMeshToAxis = -meltAxis.matrix * testModel.mTransform.matrix;
	meltBuff->matAxisToMesh = -meltBuff->matMeshToAxis;

	skydome.TransferBuffer(camera.mViewProjection);
	testModel.TransferBuffer(camera.mViewProjection);
}

void MainTestScene::Draw()
{
	skydome.Draw();

	RootSignatureDesc rootDesc = RDirectX::GetDefRootSignature().mDesc;
	rootDesc.RootParamaters.emplace_back();
	rootDesc.RootParamaters.back().ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootDesc.RootParamaters.back().Descriptor.ShaderRegister = 10; //定数バッファ番号
	rootDesc.RootParamaters.back().Descriptor.RegisterSpace = 0; //デフォルト値
	rootDesc.RootParamaters.back().ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	RootSignature rSig = RootSignature::GetOrCreate("MeltDeform", rootDesc);

	std::string id = "MeltDeform";
	
	PipelineStateDesc pipeDesc = RDirectX::GetDefPipeline().mDesc;
	pipeDesc.VS = Shader::GetOrCreate("MeltDeformVS", "./Shader/MeltDeform/MeltDeformVS.hlsl", "main", "vs_5_1");
	if (useWireframe) {
		pipeDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		id = "MeltDeformWire";
	}
	pipeDesc.pRootSignature = rSig.mPtr.Get();
	GraphicsPipeline pipe = GraphicsPipeline::GetOrCreate(id, pipeDesc);

	auto orders = testModel.GetRenderOrder();
	for (RenderOrder& order : orders) {
		order.rootData.push_back(RootData(RootDataType::SRBUFFER_CBV, meltBuff.mBuff));
		order.mRootSignature = rSig.mPtr.Get();
		order.pipelineState = pipe.mPtr.Get();
		Renderer::DrawCall("Opaque", order);
	}

	SimpleDrawer::DrawBox(100, 100, 200, 200, 0, { 1, 1, 1, 1 }, true);
	SimpleDrawer::DrawBox(120, 120, 220, 220, 0, { 1, 0, 0, 0.5f }, true);

	SimpleDrawer::DrawCircle(RWindow::GetWidth() - 30, 710, 5, 0, { 1, 0, 0, 1 }, true);
	SimpleDrawer::DrawCircle(RWindow::GetWidth() - 20, 710, 5, 0, { 0, 1, 0, 1 }, true);
	SimpleDrawer::DrawCircle(RWindow::GetWidth() - 10, 710, 5, 0, { 0, 0, 1, 1 }, true);
	SimpleDrawer::DrawCircle(RWindow::GetWidth() - 60, 710, 5, 0, { 1, 0, 0, 1 }, false);
	SimpleDrawer::DrawCircle(RWindow::GetWidth() - 50, 710, 5, 0, { 0, 1, 0, 1 }, false);
	SimpleDrawer::DrawCircle(RWindow::GetWidth() - 40, 710, 5, 0, { 0, 0, 1, 1 }, false);
	SimpleDrawer::DrawBox(RWindow::GetWidth() - 80, 705, RWindow::GetWidth() - 70, 715, 0, { 1, 0, 0, 1 }, false, 2);
}
