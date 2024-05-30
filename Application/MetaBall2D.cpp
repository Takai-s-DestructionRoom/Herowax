#include "MetaBall2D.h"
#include "RenderTarget.h"
#include "Renderer.h"
#include "RImGui.h"
#include "Util.h"
#include "Parameter.h"
#include "WaxManager.h"
#include "ColPrimitive2D.h"

MetaBall2DManager* MetaBall2DManager::GetInstance()
{
	static MetaBall2DManager instance;
	return &instance;
}

void MetaBall2DManager::Imgui()
{
	if (RImGui::showImGui)
	{
		ImGui::SetNextWindowSize({ 300, 250 }, ImGuiCond_FirstUseEver);

		ImGui::Begin("metaballRenderer");
		
		ImGui::DragInt("生成個数", &createNum);
		ImGui::DragFloat2("生成サイズ", &createSize.x, 0.1f);
		if (ImGui::Button("ランダム生成")) {
			for (int32_t i = 0; i < METABALL_NUM; i++)
			{
				metaballs[i]->isUse = false;
			}
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
		ImGui::DragFloat("移動速度", &moveSpeed, 0.1f);
		ImGui::DragFloat("重力加速", &gravityAccel, 0.1f);
		ImGui::DragFloat("跳ね返りパワー", &boundPower, 0.1f);

		if (ImGui::Button("セーブ")) {
			Parameter::Begin("metaball2D");
			Parameter::SaveColor("内側の色", color);
			Parameter::SaveColor("外側の色", strokecolor);
			Parameter::Save("stroke", stroke);
			Parameter::Save("cutoff", cutoff);
			Parameter::End();
		}

		ImGui::End();
	}
}

void MetaBall2DManager::CraeteMetaBall()
{
	for (auto& instant : metaballs)
	{
		if (!instant->isUse)
		{
			instant->Init();
			instant->sprite.Init();
			instant->sprite.SetTexture(TextureManager::Load("./Resources/Particle/particle_simple.png", "particle_simple"));
			instant->sprite.mTransform.position.x = Util::GetRand(0.f, (float)Util::WIN_WIDTH);
			instant->sprite.mTransform.position.y = Util::GetRand(0.f, (float)Util::WIN_HEIGHT);
			instant->sprite.mTransform.scale = { createSize.x,createSize.y,1 };
			instant->isUse = true;
		}
	}
}

void MetaBall2DManager::CraeteMetaBall(Vector2 pos, Vector2 size)
{
	if (!metaballs[metaball_num]->isUse)
	{
		metaballs[metaball_num]->Init();
		metaballs[metaball_num]->sprite.Init();
		metaballs[metaball_num]->sprite.SetTexture(TextureManager::Load("./Resources/Particle/particle_simple.png", "particle_simple"));
		metaballs[metaball_num]->sprite.mTransform.position = pos;
		metaballs[metaball_num]->sprite.mTransform.scale = size;
		metaballs[metaball_num]->isUse = true;
		metaball_num++;
	}
}

MetaBall2DManager::MetaBall2DManager()
{
	//レンダーターゲットを生成(1回だけ、シーンが変わっても作り直さない)
	RenderTarget::GetInstance()->CreateRenderTexture(Util::WIN_WIDTH, Util::WIN_HEIGHT,
		{0,0,0,0}, "metaballRenderer");

	for (int32_t i = 0; i < METABALL_NUM; i++)
	{
		metaballs.emplace_back();
		metaballs.back() = std::make_unique<MetaBall2D>();
	}
}

MetaBall2DManager::~MetaBall2DManager()
{

}

void MetaBall2DManager::Init()
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

	std::map<std::string, std::string> extract = Parameter::Extract("metaball2D");

	color = WaxManager::GetInstance()->slimeWax.waxColor;

	color = Parameter::GetColorData(extract,"内側の色", color);
	strokecolor = Parameter::GetColorData(extract, "外側の色", strokecolor);
	stroke = Parameter::GetParam(extract,"stroke", stroke);
	cutoff = Parameter::GetParam(extract,"cutoff", cutoff);
}

void MetaBall2DManager::Reset()
{
	metaball_num = 0;
}

void MetaBall2DManager::Update()
{
	Imgui();

	//for (auto& metaball1 : metaballs)
	//{
	//	for (auto& metaball2 : metaballs)
	//	{
	//		//同じやつなら飛ばす
	//		if (metaball1 == metaball2)continue;

	//		ColPrimitive3D::Sphere sphere1;
	//		sphere1.pos = { metaball1->GetPos() };
	//		sphere1.r = (metaball1->GetScale().x + metaball1->GetScale().y) / 2;

	//		ColPrimitive3D::Sphere sphere2;
	//		sphere2.pos = { metaball2->GetPos() };
	//		sphere2.r = (metaball2->GetScale().x + metaball2->GetScale().y) / 2;

	//		//当たってるならパワーを加算
	//		if (ColPrimitive3D::CheckSphereToSphere(sphere1, sphere2)) {
	//			metaball1->overlapPower += 0.1f;
	//			metaball2->overlapPower += 0.1f;
	//		}
	//	}
	//}

	for (auto& metaball : metaballs)
	{
		metaball->moveSpeed = moveSpeed;
		metaball->gravityAccel = gravityAccel;
		metaball->boundPower = boundPower;
				
		metaball->Update();
	}

	TransfarBuffer();
}

void MetaBall2DManager::Draw()
{
	if (renderTargetFlag)
	{
		for (auto& metaball : metaballs)
		{
 			metaball->sprite.Draw();
		}
	}
	else {
		//メタボールの元となる、ぼやけた球をmetaballRendererに描画する
		//パーティクルで使っている球を使いまわす予定だが、
		//解像度が高い球を用意して、プログラム側でぼやけ具合を調整した方が良い
		for (auto& metaball : metaballs)
		{
			for (auto& order : metaball->sprite.GetRenderOrder())
			{
				//描画先をmetaballRendererへ
				order.renderTargets = { "metaballRenderer" };

				//以下スプライトの描画処理と同一
				std::string stageID = "Sprite";
				if (metaball->sprite.mTransform.position.z < 0) {
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

void MetaBall2DManager::TransfarBuffer()
{
	metaball2DBuffer->color_ = { color.r, color.g , color.b , color.a };
	metaball2DBuffer->cutoff_ = cutoff;
	metaball2DBuffer->strokecolor_ = { strokecolor.r, strokecolor.g , strokecolor.b , strokecolor.a };
	metaball2DBuffer->stroke_ = stroke;
}

void MetaBall2D::Init()
{
	//velocity.x = Util::GetRand(-1.0f, 1.0f);
	//velocity.y = Util::GetRand(-1.0f, 1.0f);

	overlapPower = 0.00f;
	gravity = 0.0f;
}

void MetaBall2D::Update()
{
	moveVec = { 0,0,0 };

	gravity += gravityAccel;
	
	velocity.Normalize();
	moveVec += velocity * moveSpeed;
	
	moveVec.y += gravity * overlapPower;
	
	sprite.mTransform.position += moveVec;

	Vector2 pos = { sprite.mTransform.position.x, sprite.mTransform.position.y };
	Vector2 size = { sprite.mTransform.scale.x, sprite.mTransform.scale.y };
	
	if (pos.y > Util::WIN_HEIGHT - size.y) {
		
		gravity -= boundPower;
		
		//重力が足しても+なら
		if (gravity > 0){
			gravity = 0;
		}

		pos = Util::ReturnScreen(pos, size);
		sprite.mTransform.position.y = pos.y;
	}

	//if (Util::OutOfScreen(pos, size)) {
	//	//はみ出してる部分を戻す
	//	pos = Util::ReturnScreen(pos,size);
	//	sprite.mTransform.position.x = pos.x;
	//	sprite.mTransform.position.y = pos.y;

	//	//反転
	//	velocity = -velocity;
	//}
	
	sprite.mTransform.UpdateMatrix();
	sprite.TransferBuffer();
}