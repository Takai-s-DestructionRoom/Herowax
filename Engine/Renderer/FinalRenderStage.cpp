#include "FinalRenderStage.h"
#include "RDirectX.h"
#include "RenderTarget.h"
#include "RWindow.h"

void FinalRenderStage::Init()
{
	//頂点データ
	VertexPNU vertices[] = {
		{{ -1.0f, -1.0f, 0.0f}, {}, {0.0f, 1.0f}}, //左下
		{{ -1.0f, +1.0f, 0.0f}, {}, {0.0f, 0.0f}}, //左上
		{{ +1.0f, -1.0f, 0.0f}, {}, {1.0f, 1.0f}}, //右下
		{{ +1.0f, +1.0f, 0.0f}, {}, {1.0f, 0.0f}}, //右上
	};

	//頂点インデックスデータ
	uint32_t indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	mVertBuff.Init(vertices, _countof(vertices));
	mIndexBuff.Init(indices, _countof(indices));

	mRootSignature = RDirectX::GetDefRootSignature();

	// デスクリプタレンジの設定
	DescriptorRange descriptorRange{};
	descriptorRange.NumDescriptors = 1; //一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0; //テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// ルートパラメータの設定
	RootParamaters rootParams(1);
	//テクスチャレジスタ0番
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //デスクリプタテーブル
	rootParams[0].DescriptorTable = DescriptorRanges{ descriptorRange };
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える

	mRootSignature.mDesc.RootParamaters = rootParams;

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
	mRootSignature.mDesc.StaticSamplers = StaticSamplerDescs{ samplerDesc };
	mRootSignature.Create();

	mPipelineState = RDirectX::GetDefPipeline();

	mPipelineState.mDesc.VS = Shader("./Shader/Final/FinalVS.hlsl", "main", "vs_5_0");
	mPipelineState.mDesc.PS = Shader("./Shader/Final/FinalPS.hlsl", "main", "ps_5_0");

	// ラスタライザの設定
	mPipelineState.mDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	mPipelineState.mDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	mPipelineState.mDesc.RasterizerState.DepthClipEnable = false;
	mPipelineState.mDesc.BlendState.AlphaToCoverageEnable = false;

	mPipelineState.mDesc.DepthStencilState.DepthEnable = false;
	mPipelineState.mDesc.pRootSignature = mRootSignature.mPtr.Get();

	mPipelineState.Create();
}

void FinalRenderStage::Render()
{
	D3D12_VIEWPORT viewport{};
	viewport.Width = static_cast<float>(RWindow::GetWidth());
	viewport.Height = static_cast<float>(RWindow::GetHeight());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	D3D12_RECT scissorRect{};
	scissorRect.left = 0;
	scissorRect.right = scissorRect.left + RWindow::GetWidth();
	scissorRect.top = 0;
	scissorRect.bottom = scissorRect.top + RWindow::GetHeight();

	RDirectX::GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RenderTarget::SetToBackBuffer();
	RDirectX::GetCommandList()->RSSetViewports(1, &viewport);
	RDirectX::GetCommandList()->RSSetScissorRects(1, &scissorRect);

	RDirectX::GetCommandList()->SetGraphicsRootSignature(mRootSignature.mPtr.Get());
	RDirectX::GetCommandList()->SetPipelineState(mPipelineState.mPtr.Get());
	RDirectX::GetCommandList()->IASetVertexBuffers(0, 1, &mVertBuff.mView);
	RDirectX::GetCommandList()->IASetIndexBuffer(&mIndexBuff.mView);

	RDirectX::GetCommandList()->SetGraphicsRootDescriptorTable(0, TextureManager::Get(RenderTarget::GetRenderTexture("RenderingImage")->mTexHandle).mGpuHandle);
	RDirectX::GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
