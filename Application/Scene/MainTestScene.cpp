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
	testObj = ModelObj(Model::Load("./Resources/Model/Sphere.obj", "Sphere", true));

	camera.mViewProjection.mEye = { 0, 0, -30 };
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

	tankBuff->upper = 1.0f;
}

void MainTestScene::Update()
{
	ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
	ImGui::SetNextWindowSize({ 400, 500 }, ImGuiCond_FirstUseEver);

	ImGuiWindowFlags window_flags = 0;
	ImGui::Begin("TankWater Control", NULL, window_flags);
	ImGui::DragFloat3("CenterPos", &tankBuff->centerPos.x, 0.01f);
	ImGui::DragFloat("Upper", &tankBuff->upper, 0.01f);
	ImGui::DragFloat("Lower", &tankBuff->lower, 0.01f);
	ImGui::DragFloat("Amplitude", &tankBuff->amplitude, 0.01f);
	ImGui::DragFloat("Frequency", &tankBuff->frequency, 0.01f);
	ImGui::DragFloat("Timer", &tankBuff->time, 0.01f);
	ImGui::End();

	tankBuff->time += TimeManager::deltaTime;

	light.Update();
	camera.Update();

	skydome.TransferBuffer(camera.mViewProjection);
	testObj.TransferBuffer(camera.mViewProjection);
}

void MainTestScene::Draw()
{
	skydome.Draw();

	{
		for (RenderOrder order : testObj.GetRenderOrder()) {
			RenderOrder orderA = order;
			orderA.mRootSignature = GetRootSig()->mPtr.Get();
			orderA.pipelineState = GetPipeline()->mPtr.Get();
			orderA.rootData.push_back(RootData(RootDataType::SRBUFFER_CBV, tankBuff.mBuff));

			RenderOrder orderB = orderA;
			orderB.pipelineState = GetPipelineB()->mPtr.Get();
			Renderer::DrawCall("Opaque", orderA);
			Renderer::DrawCall("Opaque", orderB);
		}
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

RootSignature* MainTestScene::GetRootSig()
{
	RootSignatureDesc desc = RDirectX::GetDefRootSignature().mDesc;

	desc.RootParamaters.emplace_back();

	desc.RootParamaters.back().ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.RootParamaters.back().Descriptor.ShaderRegister = 10;
	desc.RootParamaters.back().Descriptor.RegisterSpace = 0;
	desc.RootParamaters.back().ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	return &RootSignature::GetOrCreate("TankWater", desc);
}

GraphicsPipeline* MainTestScene::GetPipeline()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().mDesc;
	desc.pRootSignature = GetRootSig()->mPtr.Get();
	desc.PS = Shader::GetOrCreate("TankWaterPS", "./Shader/TankWater/TankWaterPS.hlsl", "main", "ps_5_1");

	return &GraphicsPipeline::GetOrCreate("TankWater", desc);
}

GraphicsPipeline* MainTestScene::GetPipelineB()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().mDesc;
	desc.pRootSignature = GetRootSig()->mPtr.Get();
	desc.VS = Shader::GetOrCreate("TankWaterVS", "./Shader/TankWater/TankWaterBackVS.hlsl", "main", "vs_5_1");
	desc.PS = Shader::GetOrCreate("TankWaterPS", "./Shader/TankWater/TankWaterPS.hlsl", "main", "ps_5_1");

	desc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
	return &GraphicsPipeline::GetOrCreate("TankWaterBack", desc);
}