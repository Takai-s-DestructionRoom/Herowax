#include "SimpleDrawer.h"
#include "RDirectX.h"
#include "RWindow.h"
#include "TextDrawer.h"
#include <chrono>
#include <Renderer.h>

using namespace std;

void SimpleDrawer::ClearData()
{
	SimpleDrawer* instance = GetInstance();
	instance->mInfoList.clear();
	for (auto& itr : instance->mRecycleBuffs) {
		SRBufferAllocator::Free(*itr.second);
	}
	instance->mRecycleBuffs.clear();

	instance->mBoxInfoMap.clear();
	instance->mBoxBuffersMap.clear();
	instance->mLineInfoMap.clear();
	instance->mLineBuffersMap.clear();
	instance->mCircleInfoMap.clear();
	instance->mCircleVertIndexMap.clear();
	instance->mCircleBuffersMap.clear();
}


void SimpleDrawer::DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, float layer, Color color, float thickness)
{
	SimpleDrawer* instance = GetInstance();

	//既にそのレイヤーで描画が要求されてるか探す
	auto itr = instance->mLineInfoMap.find(layer);
	if (itr != instance->mLineInfoMap.end()) {
		//同じレイヤーに他の描画がいたら同じ描画コマンドで描画するように要素を追加する
		std::vector<DrawLineInfo>& list = itr->second;
		list.push_back({ Vector2(static_cast<float>(x1), static_cast<float>(y1)), Vector2(static_cast<float>(x2), static_cast<float>(y2)), thickness, color });
		return;
	}
	//されてないみたいなので新規描画コマンドを追加する
	instance->mLineInfoMap[layer].push_back({ Vector2(static_cast<float>(x1), static_cast<float>(y1)), Vector2(static_cast<float>(x2), static_cast<float>(y2)), thickness, color });
}

void SimpleDrawer::DrawBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2, float layer, Color color, bool fillFlag, float thickness)
{
	DrawBox(static_cast<float>(x1), static_cast<float>(y1), static_cast<float>(x2), static_cast<float>(y2), layer, color, fillFlag, thickness);
}

void SimpleDrawer::DrawBox(float x1, float y1, float x2, float y2, float layer, Color color, bool fillFlag, float thickness)
{
	SimpleDrawer* instance = GetInstance();

	if (fillFlag) {
		//4頂点ポリゴン描画

		//既にそのレイヤーで描画が要求されてるか探す
		auto itr = instance->mBoxInfoMap.find(layer);
		if (itr != instance->mBoxInfoMap.end()) {
			//同じレイヤーに他の描画がいたら同じ描画コマンドで描画するように要素を追加する
			std::vector<DrawBoxInfo>& list = itr->second;
			list.push_back({ Vector2(x1, y1), Vector2(x2, y2), color });
			return;
		}
		//されてないみたいなので新規描画コマンドを追加する
		instance->mBoxInfoMap[layer].push_back({ Vector2(x1, y1), Vector2(x2, y2), color });
	}
	else {
		//DrawLineによる描画
		int32_t adjX1 = static_cast<int32_t>(x1 + thickness / 2.0f);
		int32_t adjX2 = static_cast<int32_t>(x2 - thickness / 2.0f);
		int32_t adjY1 = static_cast<int32_t>(y1 + thickness / 2.0f);
		int32_t adjY2 = static_cast<int32_t>(y2 - thickness / 2.0f);

		SimpleDrawer::DrawLine(static_cast<int32_t>(x1), adjY1, static_cast<int32_t>(x2), adjY1, layer, color, thickness); //左上から右上
		SimpleDrawer::DrawLine(static_cast<int32_t>(x1), adjY2, static_cast<int32_t>(x2), adjY2, layer, color, thickness); //左下から右下
		SimpleDrawer::DrawLine(adjX1, static_cast<int32_t>(y1), adjX1, static_cast<int32_t>(y2), layer, color, thickness); //左上から左下
		SimpleDrawer::DrawLine(adjX2, static_cast<int32_t>(y1), adjX2, static_cast<int32_t>(y2), layer, color, thickness); //右上から右下
	}
}

void SimpleDrawer::DrawCircle(int32_t x, int32_t y, int32_t r, float layer, Color color, bool fillFlag, float thickness)
{
	SimpleDrawer* instance = GetInstance();

	//今回のカスタムデータだ
	DrawCustomData cData = { layer, fillFlag, static_cast<float>(r) };
	
	if (fillFlag) {
		//頂点とそのインデックスを必要なら計算する
		if (instance->mCircleBuffersMap.find(cData) == instance->mCircleBuffersMap.end()) {
			instance->CalcCircleVertAndIndex(cData);
		}
	}
	else {
		if (instance->mCircleBuffersMap.find(cData) == instance->mCircleBuffersMap.end()) {
			DrawCustomData cDataMin = cData; //内側の小さめな円
			DrawCustomData cDataMax = cData; //外側の大きめな円
			cDataMin.radius -= thickness / 2.0f;
			cDataMax.radius += thickness / 2.0f;

			//頂点とそのインデックスを必要なら計算する
			if (instance->mCircleBuffersMap.find(cDataMin) == instance->mCircleBuffersMap.end()) {
				instance->CalcCircleVertAndIndex(cDataMin);
			}
			if (instance->mCircleBuffersMap.find(cDataMax) == instance->mCircleBuffersMap.end()) {
				instance->CalcCircleVertAndIndex(cDataMax);
			}

			//二つの円の頂点を融合して最強にする
			std::vector<VertexP> vertices;
			std::vector<uint32_t> indices;

			std::vector<VertexP>& vertMin = instance->mCircleVertIndexMap[cDataMin].vert;
			std::vector<VertexP>& vertMax = instance->mCircleVertIndexMap[cDataMax].vert;
			for (int32_t i = 0; i < vertMin.size() - 1; i++) {
				vertices.push_back(vertMin[i]);
			}
			for (int32_t i = 0; i < vertMax.size() - 1; i++) {
				vertices.push_back(vertMax[i]);
			}

			int32_t minI = 0;
			int32_t maxI = static_cast<int32_t>(vertMin.size());
			while (minI < vertMin.size() - 1 || maxI < vertMax.size()) {
				if (minI < vertMin.size() - 1) {
					indices.push_back(minI);
					indices.push_back(minI + 1);
					indices.push_back(maxI);
					minI++;
				}
				
				if (maxI < vertices.size() - 1) {
					indices.push_back(maxI);
					indices.push_back(maxI + 1);
					indices.push_back(minI);
					maxI++;
				}
			}
			indices.push_back(0);
			indices.push_back(maxI);
			indices.push_back(minI + 1);

			instance->mCircleVertIndexMap[cData].vert = vertices;
			instance->mCircleVertIndexMap[cData].index = indices;
			instance->mCircleBuffersMap[cData].vert.Init(vertices);
			instance->mCircleBuffersMap[cData].index.Init(indices);
		}
	}

	instance->mCircleInfoMap[cData].push_back({ Vector2(x, y), color });
}

void SimpleDrawer::DrawString(float x, float y, float layer, std::string text, Color color, std::string fontTypeFace, uint32_t fontSize, Vector2 anchor)
{
	SimpleDrawer* instance = GetInstance();
	shared_ptr<DrawStringInfo> info = make_shared<DrawStringInfo>();
	TextureHandle tex = TextDrawer::CreateStringTexture(text, fontTypeFace, fontSize);
	info->sprite = Sprite(tex, anchor);
	info->sprite.mTransform.position = { x, y, layer };
	info->sprite.mTransform.UpdateMatrix();
	info->sprite.mMaterial.mColor = color;
	info->sprite.TransferBuffer();

	info->sprite.Draw();

	TextureManager::UnRegisterAtEndFrame(tex);
	instance->mInfoList.push_back(info);
}

void SimpleDrawer::DrawAll() {
	SimpleDrawer* instance = GetInstance();

	for (auto itr = instance->mBoxInfoMap.begin(); itr != instance->mBoxInfoMap.end(); itr++) {
		float layer = itr->first;

		struct vert {
			Vector2 pos;
			Color color;
		};
		std::vector<vert> vertices;
		std::vector<uint32_t> indices;
		int32_t index = 0;
		for (DrawBoxInfo& info : itr->second) {
			Matrix4 mat = Matrix4::OrthoGraphicProjection(
				0, (float)RWindow::GetWidth(),
				0, (float)RWindow::GetHeight(),
				0, 1);
			Vector3 leftTop = Vector2(info.start.x, info.start.y) - Vector2(RWindow::GetWidth() / 2.0f, RWindow::GetHeight() / 2.0f);
			Vector3 rightTop = Vector2(info.end.x, info.start.y) - Vector2(RWindow::GetWidth() / 2.0f, RWindow::GetHeight() / 2.0f);
			Vector3 leftBottom = Vector2(info.start.x, info.end.y) - Vector2(RWindow::GetWidth() / 2.0f, RWindow::GetHeight() / 2.0f);
			Vector3 rightBottom = Vector2(info.end.x, info.end.y) - Vector2(RWindow::GetWidth() / 2.0f, RWindow::GetHeight() / 2.0f);
			leftTop *= mat;
			rightTop *= mat;
			leftBottom *= mat;
			rightBottom *= mat;
			vertices.push_back({ leftTop, info.color });
			vertices.push_back({ rightTop, info.color });
			vertices.push_back({ leftBottom, info.color });
			vertices.push_back({ rightBottom, info.color });
			indices.emplace_back(index);
			indices.emplace_back(index + 3);
			indices.emplace_back(index + 1);
			indices.emplace_back(index);
			indices.emplace_back(index + 2);
			indices.emplace_back(index + 3);
			index += 4;
		}

		instance->mBoxBuffersMap[layer].vert.Update(vertices);
		instance->mBoxBuffersMap[layer].index.Init(indices);

		RenderOrder order;
		order.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		order.vertBuff = instance->mBoxBuffersMap[layer].vert;
		order.indexBuff = instance->mBoxBuffersMap[layer].index;
		order.indexCount = instance->mBoxBuffersMap[layer].index.GetIndexCount();
		order.mRootSignature = instance->mBoxRS.mPtr.Get();
		order.pipelineState = instance->mBoxPSO.mPtr.Get();
		
		string stage;
		if (layer >= 0) {
			//前面
			order.anchorPoint.z = layer;
			stage = "Sprite";
		}
		else {
			//背面
			order.anchorPoint.z = -layer;
			stage = "BackSprite";
		}
		Renderer::DrawCall(stage, order);
	}

	if (!instance->mLineInfoMap.empty()) {
		Matrix4 mat = Matrix4::OrthoGraphicProjection(
			0, (float)RWindow::GetWidth(),
			0, (float)RWindow::GetHeight(),
			0, 1);

		instance->mLineVPBuff->matrix = mat;

		for (auto itr = instance->mLineInfoMap.begin(); itr != instance->mLineInfoMap.end(); itr++) {
			float layer = itr->first;

			struct vert {
				Vector2 pos;
				Color color;
				float thickness;
			};
			std::vector<vert> vertices;
			for (DrawLineInfo& info : itr->second) {
				Vector3 start = Vector2(info.start.x, info.start.y);
				Vector3 goal = Vector2(info.end.x, info.end.y);
				vertices.push_back({ start, info.color, info.thickness });
				vertices.push_back({ goal, info.color, info.thickness });
			}

			instance->mLineBuffersMap[layer].vert.Update(vertices);

			RenderOrder order;
			order.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			order.vertBuff = instance->mLineBuffersMap[layer].vert;
			order.indexCount = static_cast<uint32_t>(vertices.size());
			order.rootData = {
				{RootDataType::SRBUFFER_CBV, instance->mLineVPBuff.mBuff }
			};
			order.mRootSignature = instance->mLineRS.mPtr.Get();
			order.pipelineState = instance->mLinePSO.mPtr.Get();

			string stage;
			if (layer >= 0) {
				//前面
				order.anchorPoint.z = layer;
				stage = "Sprite";
			}
			else {
				//背面
				order.anchorPoint.z = -layer;
				stage = "BackSprite";
			}
			Renderer::DrawCall(stage, order);
		}
	}
	
	if (!instance->mCircleInfoMap.empty()) {
		Matrix4 mat = Matrix4::OrthoGraphicProjection(
			0, (float)RWindow::GetWidth(),
			0, (float)RWindow::GetHeight(),
			0, 1);

		instance->mCircleVPBuff->matrix = mat;

		for (auto itr = instance->mCircleInfoMap.begin(); itr != instance->mCircleInfoMap.end(); itr++) {
			float layer = itr->first.layer;

			instance->mCircleBuffersMap[itr->first].instance.Update(itr->second);

			RenderOrder order;
			order.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			order.vertBuff = instance->mCircleBuffersMap[itr->first].vert;
			order.indexBuff = instance->mCircleBuffersMap[itr->first].index;
			order.indexCount = static_cast<uint32_t>(instance->mCircleBuffersMap[itr->first].index.GetIndexCount());
			order.instanceVertBuff = instance->mCircleBuffersMap[itr->first].instance;
			order.instanceCount = static_cast<uint32_t>(itr->second.size());
			order.rootData = {
				{RootDataType::SRBUFFER_CBV, instance->mCircleVPBuff.mBuff }
			};
			order.mRootSignature = instance->mCircleRS.mPtr.Get();
			order.pipelineState = instance->mCirclePSO.mPtr.Get();

			string stage;
			if (layer >= 0) {
				//前面
				order.anchorPoint.z = layer;
				stage = "Sprite";
			}
			else {
				//背面
				order.anchorPoint.z = -layer;
				stage = "BackSprite";
			}
			Renderer::DrawCall(stage, order);
		}
	}
}

void SimpleDrawer::Init()
{
	mRootSignature = RDirectX::GetDefRootSignature();

	// ルートパラメータの設定
	RootParamaters rootParams(2);
	//定数バッファ0番(Color)
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0; //定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ2番(ViewProjection)
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[1].Descriptor.ShaderRegister = 1; //定数バッファ番号
	rootParams[1].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える

	mRootSignature.mDesc.RootParamaters = rootParams;
	mRootSignature.Create();

	mPipelineState = RDirectX::GetDefPipeline();

	mPipelineState.mDesc.VS = Shader("./Shader/Simple/SimpleVS.hlsl", "main", "vs_5_0");
	mPipelineState.mDesc.PS = Shader("./Shader/Simple/SimplePS.hlsl", "main", "ps_5_0");

	// ラスタライザの設定
	mPipelineState.mDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	mPipelineState.mDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	mPipelineState.mDesc.RasterizerState.DepthClipEnable = false;

	mPipelineState.mDesc.DepthStencilState.DepthEnable = false;

	mPipelineState.mDesc.pRootSignature = mRootSignature.mPtr.Get();

	mPipelineState.Create();

	//DrawBox用
	mBoxRS.mDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	mBoxRS.Create();

	mBoxPSO.mDesc.VS = Shader("./Shader/SimpleMesh/SimpleMeshVS.hlsl", "main", "vs_5_0");
	mBoxPSO.mDesc.PS = Shader("./Shader/SimpleMesh/SimpleMeshPS.hlsl", "main", "ps_5_0");
	mBoxPSO.mDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	mBoxPSO.mDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	mBoxPSO.mDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	mBoxPSO.mDesc.RasterizerState.DepthClipEnable = true;

	{
		auto& blendstate = mBoxPSO.mDesc.BlendState.RenderTarget[0];
		blendstate.BlendEnable = true;
		blendstate.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendstate.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendstate.SrcBlendAlpha = D3D12_BLEND_ONE;
		blendstate.DestBlendAlpha = D3D12_BLEND_ZERO;
		blendstate.BlendOp = D3D12_BLEND_OP_ADD;
		blendstate.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendstate.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	}

	mBoxPSO.mDesc.InputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	mBoxPSO.mDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	mBoxPSO.mDesc.DepthStencilState.DepthEnable = false;
	mBoxPSO.mDesc.NumRenderTargets = 1;
	mBoxPSO.mDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	mBoxPSO.mDesc.SampleDesc.Count = 1;
	mBoxPSO.mDesc.pRootSignature = mBoxRS.mPtr.Get();
	mBoxPSO.Create();

	//DrawLine用
	mLineRS.mDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	RootParamaters lineRootParams(1);
	lineRootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	lineRootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	lineRootParams[0].Descriptor.RegisterSpace = 0;
	lineRootParams[0].Descriptor.ShaderRegister = 0;
	mLineRS.mDesc.RootParamaters = lineRootParams;
	mLineRS.Create();

	mLinePSO.mDesc.VS = Shader("./Shader/SimpleLine/SimpleLineVS.hlsl", "main", "vs_5_0");
	mLinePSO.mDesc.GS = Shader("./Shader/SimpleLine/SimpleLineGS.hlsl", "main", "gs_5_0");
	mLinePSO.mDesc.PS = Shader("./Shader/SimpleLine/SimpleLinePS.hlsl", "main", "ps_5_0");
	mLinePSO.mDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	mLinePSO.mDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	mLinePSO.mDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	mLinePSO.mDesc.RasterizerState.DepthClipEnable = true;

	{
		auto& blendstate = mLinePSO.mDesc.BlendState.RenderTarget[0];
		blendstate.BlendEnable = true;
		blendstate.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendstate.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendstate.SrcBlendAlpha = D3D12_BLEND_ONE;
		blendstate.DestBlendAlpha = D3D12_BLEND_ZERO;
		blendstate.BlendOp = D3D12_BLEND_OP_ADD;
		blendstate.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendstate.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	}

	mLinePSO.mDesc.InputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"THICKNESS", 0, DXGI_FORMAT_R32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}
	};

	mLinePSO.mDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	mLinePSO.mDesc.DepthStencilState.DepthEnable = false;
	mLinePSO.mDesc.NumRenderTargets = 1;
	mLinePSO.mDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	mLinePSO.mDesc.SampleDesc.Count = 1;
	mLinePSO.mDesc.pRootSignature = mLineRS.mPtr.Get();
	mLinePSO.Create();

	//DrawCircle用
	mCircleRS.mDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	RootParamaters circleRootParams(1);
	circleRootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	circleRootParams[0].Descriptor.ShaderRegister = 0; //定数バッファ番号
	circleRootParams[0].Descriptor.RegisterSpace = 0; //デフォルト値
	circleRootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	mCircleRS.mDesc.RootParamaters = circleRootParams;
	mCircleRS.mDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	mCircleRS.Create();

	mCirclePSO.mDesc.VS = Shader("Shader/SimpleInstance/SimpleInstanceVS.hlsl", "main", "vs_5_0");
	mCirclePSO.mDesc.PS = Shader("Shader/SimpleInstance/SimpleInstancePS.hlsl", "main", "ps_5_0");
	mCirclePSO.mDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	mCirclePSO.mDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	mCirclePSO.mDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	mCirclePSO.mDesc.RasterizerState.DepthClipEnable = true;

	{
		auto& blendstate = mCirclePSO.mDesc.BlendState.RenderTarget[0];
		blendstate.BlendEnable = true;
		blendstate.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendstate.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendstate.SrcBlendAlpha = D3D12_BLEND_ONE;
		blendstate.DestBlendAlpha = D3D12_BLEND_ZERO;
		blendstate.BlendOp = D3D12_BLEND_OP_ADD;
		blendstate.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendstate.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	}

	mCirclePSO.mDesc.InputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"POSITION", 1, DXGI_FORMAT_R32G32_FLOAT, 1,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1
		},
		{
			"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1
		},
	};

	mCirclePSO.mDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	mCirclePSO.mDesc.DepthStencilState.DepthEnable = false;
	mCirclePSO.mDesc.NumRenderTargets = 1;
	mCirclePSO.mDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	mCirclePSO.mDesc.SampleDesc.Count = 1;
	mCirclePSO.mDesc.pRootSignature = mLineRS.mPtr.Get();
	mCirclePSO.Create();

	//DrawString用
	mRootSignatureForString = SpriteManager::GetInstance()->GetRootSignature();

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
	mRootSignatureForString.mDesc.StaticSamplers = StaticSamplerDescs{ samplerDesc };
	mRootSignatureForString.Create();

	mPipelineStateForString = SpriteManager::GetInstance()->GetGraphicsPipeline();
	mPipelineStateForString.mDesc.pRootSignature = mRootSignatureForString.mPtr.Get();
	mPipelineStateForString.Create();
}

void SimpleDrawer::CalcCircleVertAndIndex(DrawCustomData cData) {
	SimpleDrawer* instance = GetInstance();

	std::vector<VertexP> vertices;
	std::vector<uint32_t> indices;

	float ang = 0;
	for (int32_t i = 0; i < 360; i++) {
		float plus = 360.0f / 360;
		vertices.push_back(VertexP({ cData.radius * cosf(Util::AngleToRadian(ang)), cData.radius * sinf(Util::AngleToRadian(ang)), 0 }));
		vertices.push_back(VertexP({ cData.radius * cosf(Util::AngleToRadian(ang + plus)), cData.radius * sinf(Util::AngleToRadian(ang + plus)), 0 }));
		ang += plus;
	}
	vertices.push_back(VertexP({ 0, 0, 0 }));

	for (int32_t i = 0; i < vertices.size() - 2; i++) {
		indices.push_back(i);
		indices.push_back(i + 1);
		indices.push_back(static_cast<uint32_t>(vertices.size() - 1));
	}

	instance->mCircleVertIndexMap[cData].vert = vertices;
	instance->mCircleVertIndexMap[cData].index = indices;
	instance->mCircleBuffersMap[cData].vert.Init(vertices);
	instance->mCircleBuffersMap[cData].index.Init(indices);
}
