#include "MetaBall2D.h"
#include "RenderTarget.h"
#include "Renderer.h"
#include "RImGui.h"
#include "Util.h"

MetaBall2D* MetaBall2D::GetInstance()
{
	static MetaBall2D instance;
	return &instance;
}

void MetaBall2D::Imgui()
{
	if (RImGui::showImGui)
	{
		ImGui::SetNextWindowSize({ 300, 250 }, ImGuiCond_FirstUseEver);

		ImGui::Begin("metaballRenderer");
		
		ImGui::DragInt("生成個数", &createNum);
		ImGui::DragFloat2("生成サイズ", &createSize.x, 0.1f);
		if (ImGui::Button("ランダム生成")) {
			metaballs.clear();
			for (int32_t i = 0; i < createNum; i++)
			{
				CraeteMetaBall();
			}
		}

		ImGui::Text("メタボール_個数:%d", (int32_t)metaballs.size());
		ImGui::Checkbox("描画先切り替え", &renderTargetFlag);
		ImGui::ColorEdit4("内側の色", &color.r);
		ImGui::ColorEdit4("外側の色", &strokecolor.r);
		ImGui::SliderFloat("stroke", &stroke, 0.f, 1.0f);
		ImGui::SliderFloat("cutoff", &cutoff, 0.f, 1.0f);

		ImGui::End();
	}
}

void MetaBall2D::CraeteMetaBall()
{
	metaballs.emplace_back();
	metaballs.back().Init();
	metaballs.back().SetTexture(TextureManager::Load("./Resources/Particle/particle_simple.png", "particle_simple"));
	metaballs.back().mTransform.position.x = Util::GetRand(0.f, (float)Util::WIN_WIDTH);
	metaballs.back().mTransform.position.y = Util::GetRand(0.f, (float)Util::WIN_HEIGHT);
	metaballs.back().mTransform.scale = { createSize.x,createSize.y,1 };
}

MetaBall2D::MetaBall2D()
{
	//レンダーターゲットを生成(1回だけ、シーンが変わっても作り直さない)
	RenderTarget::GetInstance()->CreateRenderTexture(Util::WIN_WIDTH, Util::WIN_HEIGHT,
		{0,0,0,0}, "metaballRenderer");
}

MetaBall2D::~MetaBall2D()
{

}

void MetaBall2D::Init()
{
	struct metaballVert 
	{
		Vector3 pos;
		Vector2 uv;
	};

	//頂点データ
	metaballVert vertices[] = {
		{{-1,-1,0},{0,0}},	//左上
		{{-1,1,0},{0,1}},	//左下
		{{1,-1,0},{1,0}},	//右上
		{{1,1,0},{1,1}},	//右下
	};

	//頂点インデックスデータ
	uint32_t indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	mVertBuff.Init(vertices, _countof(vertices));
	mIndexBuff.Init(indices, _countof(indices));

	mRootSignature = SpriteManager::GetInstance()->GetRootSignature();
	mPipelineState = SpriteManager::GetInstance()->GetGraphicsPipeline();

	mPipelineState.mDesc.VS = Shader("./Shader/MetaballRenderer/MetaballRendererVS.hlsl", "main", "vs_5_0");
	mPipelineState.mDesc.PS = Shader("./Shader/MetaballRenderer/MetaballRendererPS.hlsl", "main", "ps_5_0");

	// グラフィックスパイプライン設定
	InputLayout inputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}
	};

	// 頂点レイアウトの設定
	mPipelineState.mDesc.InputLayout = inputLayout;

	mRootSignature.mDesc.RootParamaters.clear();

	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//パラメータの追加
	RootParamater metaball2DTexture{};
	metaball2DTexture.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //デスクリプタテーブル
	metaball2DTexture.DescriptorTable = DescriptorRanges{ descriptorRange };
	metaball2DTexture.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	mRootSignature.mDesc.RootParamaters.push_back(metaball2DTexture);

	//パラメータの追加
	RootParamater metaball2DRP{};
	metaball2DRP.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	metaball2DRP.Descriptor.ShaderRegister = 0;
	metaball2DRP.Descriptor.RegisterSpace = 0;
	metaball2DRP.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	mRootSignature.mDesc.RootParamaters.push_back(metaball2DRP);

	mPipelineState.mDesc.pRootSignature = mRootSignature.mPtr.Get();

	//書き換えたので作り直し
	mPipelineState.Create();
}

void MetaBall2D::Update()
{
	Imgui();

	for (auto& metaball : metaballs)
	{
		metaball.mTransform.UpdateMatrix();
		metaball.TransferBuffer();
	}

	TransfarBuffer();
}

void MetaBall2D::Draw()
{
	if (renderTargetFlag)
	{
		for (auto& metaball : metaballs)
		{
			metaball.Draw();
		}
	}
	else {
		
		//メタボールの元となる、ぼやけた球をmetaballRendererに描画する
		//パーティクルで使っている球を使いまわす予定だが、
		//解像度が高い球を用意して、プログラム側でぼやけ具合を調整した方が良い
		for (auto& metaball : metaballs)
		{
			for (auto& order : metaball.GetRenderOrder())
			{
				//描画先をmetaballRendererへ
				order.renderTargets = { "metaballRenderer" };

				//以下スプライトの描画処理と同一
				std::string stageID = "Sprite";
				if (metaball.mTransform.position.z < 0) {
					stageID = "BackSprite";
				}
				Renderer::DrawCall(stageID, order);
			}
		}

		
		//metaballRendererをSRVとして見て、シェーダーを元にスプライトに書き込む
		RenderTexture* tex = RenderTarget::GetInstance()->GetRenderTexture("metaballRenderer");
		tex->ClearRenderTarget();

		RenderOrder order;

		order.anchorPoint = {0,0,0};
		order.vertBuff = mVertBuff;
		order.indexBuff = mIndexBuff;
		order.indexCount = 6;

		//metaballRendererをSRVとして見る(0番がテクスチャ)
		//後ろにバッファを入れる
		order.rootData = {
			{tex->GetTexture().mGpuHandle},
			{RootDataType::SRBUFFER_CBV, metaball2DBuffer.mBuff },
		};

		order.mRootSignature = GetRootSignature().mPtr.Get();
		order.pipelineState = GetGraphicsPipeline().mPtr.Get();
		
		//以下スプライトの描画処理と同一
		std::string stageID = "Sprite";
		
		Renderer::DrawCall(stageID, order);
	}
}

void MetaBall2D::TransfarBuffer()
{
	metaball2DBuffer->color_ = { color.r, color.g , color.b , color.a };
	metaball2DBuffer->cutoff_ = cutoff;
	metaball2DBuffer->strokecolor_ = { strokecolor.r, strokecolor.g , strokecolor.b , strokecolor.a };
	metaball2DBuffer->stroke_ = stroke;
}
