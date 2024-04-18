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

	std::vector<VertexPU> verts;
	verts.push_back({ {-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f} });
	verts.push_back({ {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f} });
	verts.push_back({ {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f} });
	verts.push_back({ {1.0f, -1.0f, 0.0f}, {1.0f, 1.0f} });

	std::vector<uint32_t> indices = { 0, 1, 2, 1, 3, 2 };
	vertBuff.Init(verts);
	indexBuff.Init(indices);

	for (int32_t i = 0; i < sphereCreateNum; i++) {
		spheres.push_back({ Util::GetRand(-15.0f, 15.0f), Util::GetRand(-15.0f, 15.0f), Util::GetRand(-15.0f, 15.0f), Util::GetRand(1.0f, 3.0f) });
	}
	for (size_t i = 0; i < spheres.size(); i++) {
		constBuff->spheres[i] = spheres[i];
	}

	rtTex = RenderTarget::CreateRenderTexture(RWindow::GetWidth() / 4, RWindow::GetHeight() / 4, { 0, 0, 0, 0 }, "MainTestScene_RT");
}

MainTestScene::~MainTestScene()
{
	//RenderTarget::DeleteRenderTextureAtEndFrame(rtTex);
}

void MainTestScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
}

void MainTestScene::Update()
{
	ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
	ImGui::SetNextWindowSize({ 400, 500 }, ImGuiCond_FirstUseEver);

	ImGuiWindowFlags window_flags = 0;
	ImGui::Begin("RayMarch Control", NULL, window_flags);
	ImGui::DragInt("RayMatchNum", reinterpret_cast<int32_t*>(&constBuff->rayMatchNum), 1, 0, INT_MAX);
	ImGui::DragFloat("HitThreshold", &constBuff->hitThreshold, 0.001f, 0, FLT_MAX);
	ImGui::DragFloat("SmoothFactor", &constBuff->smoothFactor, 0.01f, 0, FLT_MAX);
	ImGui::DragInt("SphereCreateNum", &sphereCreateNum);
	ImGui::Separator();
	ImGui::DragFloat("sigma", &blurBuff->sigma, 0.001f);
	ImGui::DragFloat("stepwidth", &blurBuff->stepwidth, 0.001f, 0.001f, 114514.0f, "%.3f", ImGuiSliderFlags_ClampOnInput);
	if (ImGui::Button("再生成")) {
		spheres.clear();
		for (int32_t i = 0; i < sphereCreateNum; i++) {
			spheres.push_back({ Util::GetRand(-15.0f, 15.0f), Util::GetRand(-15.0f, 15.0f), Util::GetRand(-15.0f, 15.0f), Util::GetRand(1.0f, 3.0f) });
		}
		for (size_t i = 0; i < spheres.size(); i++) {
			constBuff->spheres[i] = spheres[i];
		}
	}
	ImGui::End();

	light.Update();
	camera.Update();

	cameraBuff->pos = camera.mViewProjection.mEye;
	cameraBuff->dir = (camera.mViewProjection.mTarget - camera.mViewProjection.mEye).GetNormalize();
	cameraBuff->up = camera.mViewProjection.mUpVec;
	cameraBuff->matViewProj = camera.mViewProjection.mMatrix;
	cameraBuff->matInvView = -camera.mViewProjection.mView;
	cameraBuff->matInvProj = -camera.mViewProjection.mProjection;
	cameraBuff->matInvViewport = -Matrix4::Viewport(0, 0, 1280, 720, 0.0f, 1.0f);

	if (RInput::GetKeyDown(DIK_R)) {
		spheres.clear();
		for (int32_t i = 0; i < sphereCreateNum; i++) {
			spheres.push_back({ Util::GetRand(-15.0f, 15.0f), Util::GetRand(-15.0f, 15.0f), Util::GetRand(-15.0f, 15.0f), Util::GetRand(1.0f, 3.0f) });
		}
		for (size_t i = 0; i < spheres.size(); i++) {
			constBuff->spheres[i] = spheres[i];
		}
	}

	constBuff->sphereNum = static_cast<uint32_t>(spheres.size());

	skydome.TransferBuffer(camera.mViewProjection);
	testObj.TransferBuffer(camera.mViewProjection);

	sprite.SetTexture(rtTex->mTexHandle);
	sprite.SetTexRect(0, 0, RWindow::GetWidth() / 4, RWindow::GetHeight() / 4);
	sprite.SetAnchor({ 0, 0 });
	sprite.mTransform.scale = { 4, 4, 1 };
	sprite.mTransform.UpdateMatrix();
	sprite.TransferBuffer();
}

void MainTestScene::Draw()
{
	skydome.Draw();
	testObj.Draw();
	if (RInput::GetKey(DIK_T)) {
		sprite.Draw();
	}

	rtTex->ClearRenderTarget();
	rtTex->ClearDepthStencil();

	Viewport vp;
	vp.topleftX = 0;
	vp.topleftY = 0;
	vp.width = (float)RWindow::GetWidth() / 4;
	vp.height = (float)RWindow::GetHeight() / 4;
	vp.minDepth = 0;
	vp.maxDepth = 1;

	RRect sr;
	sr.top = 0;
	sr.left = 0;
	sr.right = RWindow::GetWidth() / 4;
	sr.bottom = RWindow::GetHeight() / 4;

	RenderOrder order;
	order.renderTargets = { rtTex->mName };
	order.vertBuff = vertBuff;
	order.indexBuff = indexBuff;
	order.indexCount = 6;
	order.rootData = {
		{ RootDataType::SRBUFFER_CBV, materialBuff.mBuff },
		{ RootDataType::SRBUFFER_CBV, cameraBuff.mBuff },
		{ RootDataType::LIGHT },
		{ RootDataType::SRBUFFER_CBV, constBuff.mBuff }
	};
	order.viewports = { vp };
	order.scissorRects = { sr };
	order.mRootSignature = GetRootSig()->mPtr.Get();
	order.pipelineState = GetPipeline()->mPtr.Get();

	RenderOrder orderB;
	orderB.vertBuff = vertBuff;
	orderB.indexBuff = indexBuff;
	orderB.indexCount = 6;
	orderB.rootData = {
		{ rtTex->GetTexture().mGpuHandle },
		{ rtTex->GetDepthTexture().mGpuHandle },
		{ RootDataType::SRBUFFER_CBV, blurBuff.mBuff }
	};
	orderB.mRootSignature = GetRootSigB()->mPtr.Get();
	orderB.pipelineState = GetPipelineB()->mPtr.Get();

	order.postOrder.push_back(orderB);

	Renderer::DrawCall("Transparent", order);

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
	RootSignatureDesc desc{};
	desc.RootParamaters = RootParamaters(4);

	desc.RootParamaters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.RootParamaters[0].Descriptor.ShaderRegister = 0;
	desc.RootParamaters[0].Descriptor.RegisterSpace = 0;
	desc.RootParamaters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	desc.RootParamaters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.RootParamaters[1].Descriptor.ShaderRegister = 1;
	desc.RootParamaters[1].Descriptor.RegisterSpace = 0;
	desc.RootParamaters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	desc.RootParamaters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.RootParamaters[2].Descriptor.ShaderRegister = 2;
	desc.RootParamaters[2].Descriptor.RegisterSpace = 0;
	desc.RootParamaters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	desc.RootParamaters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.RootParamaters[3].Descriptor.ShaderRegister = 3;
	desc.RootParamaters[3].Descriptor.RegisterSpace = 0;
	desc.RootParamaters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	return &RootSignature::GetOrCreate("RayMarchWax", desc);
}

GraphicsPipeline* MainTestScene::GetPipeline()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().mDesc;
	desc.pRootSignature = GetRootSig()->mPtr.Get();
	desc.VS = Shader::GetOrCreate("RayMarchWaxVS", "./Shader/SlimeShader/WaxShaderVS.hlsl", "main", "vs_5_1");
	desc.PS = Shader::GetOrCreate("RayMarchWaxPS", "./Shader/SlimeShader/WaxShaderPS.hlsl", "main", "ps_5_1");

	desc.InputLayout = {
		{
			"SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}
	};
	return &GraphicsPipeline::GetOrCreate("RayMarchWax", desc);
}

RootSignature* MainTestScene::GetRootSigB()
{
	RootSignatureDesc desc{};
	desc.RootParamaters = RootParamaters(3);

	desc.RootParamaters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	desc.RootParamaters[0].DescriptorTable = RootParamater::OneTex(0);
	desc.RootParamaters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	desc.RootParamaters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	desc.RootParamaters[1].DescriptorTable = RootParamater::OneTex(1);
	desc.RootParamaters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	desc.RootParamaters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.RootParamaters[2].Descriptor.ShaderRegister = 0;
	desc.RootParamaters[2].Descriptor.RegisterSpace = 0;
	desc.RootParamaters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	StaticSamplerDesc samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; //ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //リニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX; //ミップマップ最大値
	samplerDesc.MinLOD = 0.0f; //ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //ピクセルシェーダーからだけ見える
	desc.StaticSamplers = StaticSamplerDescs{ samplerDesc };

	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	return &RootSignature::GetOrCreate("RayMarchWaxMerge", desc);
}

GraphicsPipeline* MainTestScene::GetPipelineB()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().mDesc;
	desc.pRootSignature = GetRootSigB()->mPtr.Get();
	desc.VS = Shader::GetOrCreate("RayMarchWaxMergeVS", "./Shader/SlimeShader/WaxMergeVS.hlsl", "main", "vs_5_1");
	desc.PS = Shader::GetOrCreate("RayMarchWaxMergePS", "./Shader/SlimeShader/WaxMergePS.hlsl", "main", "ps_5_1");

	desc.InputLayout = {
		{
			"SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}
	};
	return &GraphicsPipeline::GetOrCreate("RayMarchWaxMerge", desc);
}
