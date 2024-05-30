#include "WaxWall.h"
#include "WaxManager.h"
#include "RImGui.h"
#include <Renderer.h>
#include <SimpleDrawer.h>

void WaxWall::Init()
{
	obj = PaintableModelObj(Model::Load("./Resources/Model/playerShield/playerShield.obj", "playerShield"));
	obj.mTransform.scale = { 2,2,2 };

	colliderSize = 10;

	isAlive = false;

	obj.mTuneMaterial.mColor = Color::kWaxColor;
	obj.mTuneMaterial.mColor.a = { 1.0f };

	GetWaxWallRootSig();
	GetWaxWallPipeline();
}

void WaxWall::Update()
{
	//パリィ中の場合はパリィが終わった後に終了を呼ぶ
	if (endFlag && parryTimer.GetEnd()) {
		End();
		endFlag = false;
	}

	parryTimer.Update();
	
	if (parryTimer.GetNowEnd()) {
		leakOutTimer.Start();
	}

	leakOutTimer.Update();

	if (!isAlive)return;

	//入りの挙動(外側から濃くなって、プレイヤーの周りに発生する)
	obj.mTransform.scale = { 0, 0, 0 };
	if (parryTimer.GetStarted()) {
		float f = 1 + 2.7f * powf(parryTimer.GetTimeRate() - 1, 3) + 1.7f * powf(parryTimer.GetTimeRate() - 1, 2);
		
		float barriarScale = 8.0f * f;
		SimpleDrawer::DrawString(0, 0, 0, Util::StringFormat("%f, %f", parryTimer.GetTimeRate(), barriarScale));
		obj.mTransform.scale += { barriarScale, barriarScale, barriarScale };
	}
	obj.mTuneMaterial.mColor.a = Easing::OutQuad(0.5f, 0.6f, parryTimer.GetTimeRate());

	UpdateCollider();

	if (GetParry()) {
		hogeBuff->rimColor = Color::kLightblue;
	}
	else {
		hogeBuff->rimColor = Color::kWhite;
	}	
	hogeBuff->waxColor = Color::kWaxColor;
	hogeBuff->waxColor.a = 0.1f;
	hogeBuff->rimPower = 1.0f;

	obj.mTransform.UpdateMatrix();
	BrightTransferBuffer(Camera::sNowCamera->mViewProjection);


	if (RImGui::showImGui)
	{
		ImGui::SetNextWindowSize({ 600, 250 }, ImGuiCond_FirstUseEver);

		ImGui::Begin("WaxWall");
		ImGui::ColorEdit4("色", &obj.mTuneMaterial.mColor.r);
		ImGui::End();
	}
}

void WaxWall::Draw()
{
	if (!isAlive)return;

	for (RenderOrder order : obj.GetRenderOrder()) {
		order.mRootSignature = GetWaxWallRootSig()->mPtr.Get();
		order.pipelineState = GetWaxWallPipeline()->mPtr.Get();

		order.rootData.push_back({ RootDataType::SRBUFFER_CBV, hogeBuff.mBuff });

		Renderer::DrawCall("Opaque", order);
	}
}

bool WaxWall::GetParry()
{
	return parryTimer.GetRun();
}

bool WaxWall::GetLeakOutMode()
{
	return parryTimer.GetEnd();
}

bool WaxWall::ContinueCheck(int32_t remainWax)
{
	//漏れるタイマーが終了して
	if (leakOutTimer.GetEnd()) {
		//ロウの消費ができるなら継続
		if(remainWax >= CONTINUE_CHECK_WAXNUM){
			leakOutTimer.Start();
			return true;
		}
		//出来ないなら終了
		else {
			End();
		}
	}
	return false;
}

bool WaxWall::StartCheck(int32_t remainWax)
{
	return remainWax >= START_CHECK_WAXNUM;
}

void WaxWall::Start()
{
	parryTimer.Start();
	isAlive = true;
	endFlag = false;
}

void WaxWall::End()
{
	if (parryTimer.GetEnd()) {
		isAlive = false;
		leakOutTimer.Reset();
		parryTimer.Reset();
	}
	else {
		endFlag = true;
	}
}

RootSignature* WaxWall::GetWaxWallRootSig()
{
	RootSignatureDesc desc = PaintableModelObj::GetRootSig()->mDesc;

	desc.RootParamaters.emplace_back();

	desc.RootParamaters.back().ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	desc.RootParamaters.back().Descriptor.ShaderRegister = 4;
	desc.RootParamaters.back().Descriptor.RegisterSpace = 0;
	desc.RootParamaters.back().ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	return &RootSignature::GetOrCreate("WaxWall", desc);
}

GraphicsPipeline* WaxWall::GetWaxWallPipeline()
{
	PipelineStateDesc desc = RDirectX::GetDefPipeline().mDesc;
	desc.pRootSignature = GetWaxWallRootSig()->mPtr.Get();
	desc.VS = Shader::GetOrCreate("WaxWallVS", "./Shader/SlimeWax_Light/SlimeWaxLightVS.hlsl", "main", "vs_5_0");
	desc.PS = Shader::GetOrCreate("WaxWallPS", "./Shader/SlimeWax_Light/SlimeWaxLightPS.hlsl", "main", "ps_5_0");

	desc.BlendState.AlphaToCoverageEnable = false;
	desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	return &GraphicsPipeline::GetOrCreate("WaxWall", desc);
}
